/*
 * $Id: doswin.cxx,v 1.3 1995/03/25 02:09:11 robertj Exp $
 *
 * Portable Windows Library
 *
 * Operating System Classes Implementation
 *
 * Copyright 1993 by Robert Jongbloed and Craig Southeren
 *
 * $Log: doswin.cxx,v $
 * Revision 1.3  1995/03/25 02:09:11  robertj
 * Added check for network login name.
 *
// Revision 1.2  1995/03/14  13:31:36  robertj
// Implemented DOS pipe channel.
//
// Revision 1.1  1995/03/14  12:45:16  robertj
// Initial revision
//
 */

#include "ptlib.h"

#include <fcntl.h>
#include <sys/stat.h>


///////////////////////////////////////////////////////////////////////////////
// Directories

void PDirectory::Construct()
{
  PString::operator=(CreateFullPath(*this, TRUE));
}


BOOL PDirectory::Open(int newScanMask)
{
  scanMask = newScanMask;

  if (_dos_findfirst(*this+"*.*", 0xff, &fileinfo) != 0)
    return FALSE;

  return Filtered() ? Next() : TRUE;
}


BOOL PDirectory::Next()
{
  do {
    if (_dos_findnext(&fileinfo) != 0)
      return FALSE;
  } while (Filtered());

  return TRUE;
}


PCaselessString PDirectory::GetEntryName() const
{
  return fileinfo.name;
}


BOOL PDirectory::IsSubDir() const
{
  return (fileinfo.attrib&_A_SUBDIR) != 0;
}


void PDirectory::Close()
{
  /* do nothing */
}


PCaselessString PDirectory::GetVolume() const
{
  struct find_t finf;
  if (_dos_findfirst(Left(3) + "*.*", _A_VOLID, &finf) != 0)
    return PCaselessString();
  return finf.name;
}


PString PDirectory::CreateFullPath(const PString & path, BOOL isDirectory)
{
  PString curdir;
  PAssert(getcwd(curdir.GetPointer(P_MAX_PATH),
                                   P_MAX_PATH) != NULL, POperatingSystemError);

  PString fullpath;

  PINDEX offset;
  if (path.GetLength() < 2 || path[1] != ':') {
    fullpath = curdir(0,1);
    offset = 0;
  }
  else {
    fullpath = path(0,1).ToUpper();
    offset = 2;
  }

  char slash = path[offset];
  if (slash != '\\' && slash != '/') {
    if (fullpath[0] == curdir[0])
      fullpath += curdir(2, P_MAX_INDEX);
    else if (_chdrive(fullpath[0]-'A'+1) == 0) {
      PString otherdir;
      PAssert(getcwd(otherdir.GetPointer(P_MAX_PATH),
                                   P_MAX_PATH) != NULL, POperatingSystemError);
      fullpath += otherdir(2, P_MAX_INDEX);
      _chdrive(curdir[0]-'A'+1);  // Put drive back
    }
    slash = fullpath[fullpath.GetLength()-1];
    if (slash != '\\' && slash != '/')
      fullpath += "\\";
  }

  fullpath += path(offset, P_MAX_INDEX);

  slash = fullpath[fullpath.GetLength()-1];
  if (isDirectory && slash != '\\' && slash != '/')
    fullpath += "\\";

  int pos;
  while ((pos = fullpath.Find('/')) != P_MAX_INDEX)
    fullpath[pos] = '\\';

  while ((pos = fullpath.Find("\\.\\")) != P_MAX_INDEX)
    fullpath = fullpath(0, pos) + fullpath(pos+3, P_MAX_INDEX);

  while ((pos = fullpath.Find("\\..\\")) != P_MAX_INDEX)
    fullpath = fullpath(0, fullpath.FindLast('\\', pos-1)) +
                                                  fullpath(pos+4, P_MAX_INDEX);

  return fullpath.ToUpper();
}


///////////////////////////////////////////////////////////////////////////////
// PPipeChannel

void PPipeChannel::Construct(const PString & subProgram,
                const char * const * arguments, OpenMode mode, BOOL searchPath)
{
  hasRun = FALSE;

  if (searchPath || subProgram.FindOneOf(":\\/") != P_MAX_INDEX)
    subProgName = subProgram;
  else
    subProgName = ".\\" + subProgram;
  if (arguments != NULL) {
    while (*arguments != NULL) {
      subProgName += " ";
      if (strchr(*arguments, ' ') == NULL)
        subProgName += *arguments;
      else {
        PString quote = '"';
        subProgName += quote + *arguments + quote;
      }
    }
  }
  
  if (mode != ReadOnly) {
    toChild = PFilePath("pw", NULL);
    os_handle = _open(toChild, _O_WRONLY|_O_CREAT|_O_BINARY,S_IREAD|S_IWRITE);
    if (!ConvertOSError(os_handle))
      return;
    subProgName += '<' + toChild;
  }

  if (mode != WriteOnly) {
    fromChild = PFilePath("pw", NULL);
    subProgName += '>' + fromChild;
  }

  if (mode == ReadOnly)
    Execute();
}


void PPipeChannel::DestroyContents()
{
  Close();
}


void PPipeChannel::CloneContents(const PPipeChannel *)
{
  PAssertAlways("Cannot clone pipe");
}


void PPipeChannel::CopyContents(const PPipeChannel & chan)
{
  toChild = chan.toChild;
  fromChild = chan.fromChild;
  hasRun = chan.hasRun;
}


BOOL PPipeChannel::Read(void * buffer, PINDEX amount)
{
  if (!hasRun)
    Execute();

  flush();
  lastReadCount = _read(GetHandle(), buffer, amount);
  return ConvertOSError(lastReadCount) && lastReadCount > 0;
}
      

BOOL PPipeChannel::Write(const void * buffer, PINDEX amount)
{
  if (hasRun) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  flush();
  lastWriteCount = _write(GetHandle(), buffer, amount);
  return ConvertOSError(lastWriteCount) && lastWriteCount >= amount;
}


BOOL PPipeChannel::Close()
{
  if (!hasRun)
    Execute();

  if (os_handle >= 0)
    _close(os_handle);

  PFile::Remove(toChild);
  PFile::Remove(fromChild);
  return TRUE;
}


BOOL PPipeChannel::Execute()
{
  if (hasRun)
    return FALSE;

  flush();
  if (os_handle >= 0) {
    _close(os_handle);
    os_handle = -1;
  }

#if defined(_WINDOWS)
  if ((osError = (int)WinExec(subProgName, SW_HIDE)) < 32) {
    switch (osError) {
      case 0 :
      case 8 :
        osError = ENOMEM;
        break;
      case 5 :
        osError = EACCES;
        break;
      case 2 :
        break;
      default :
        osError += 0x4000;
    }
    return ConvertOSError(-2);
  }
#else
  if (!ConvertOSError(system(subProgName)))
    return FALSE;
#endif

  if (!fromChild.IsEmpty()) {
    os_handle = _open(fromChild, _O_RDONLY);
    if (!ConvertOSError(os_handle))
      return FALSE;
  }

  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// PThread

PThread::~PThread()
{
  Terminate();
  _nfree(stackBase);   // Give stack back to the near heap
}


void PThread::Block(BlockFunction isBlockFun, PObject * obj)
{
  isBlocked = isBlockFun;
  blocker = obj;
  status = Blocked;
  Yield();
}


///////////////////////////////////////////////////////////////////////////////
// PProcess

void PProcess::OperatingSystemYield()
{
}



PString PProcess::GetUserName() const
{
  /* ----- Microsoft LAN Manager, Windows for Workgroups, IBM LAN Server ----- */
#pragma pack(1)
  static struct {
    char _far *computername;
    char _far *username;
    char _far *langroup;
    unsigned char ver_major;
    unsigned char ver_minor;
    char _far *logon_domain;
    char _far *oth_domains;
    char filler[32];
  } NEAR wksta;
#pragma pack()

  union REGS r;
  r.x.ax = 0x5F44;
  r.x.bx = 10;
  r.x.cx = sizeof(wksta);
  r.x.di = (WORD)&wksta;

  struct SREGS sregs;
  segread(&sregs);
  sregs.es = sregs.ds;
  int86x(0x21, &r, &r, &sregs);
  if (r.x.ax == 0 || r.x.ax == 0x5F44) {
    char name[32];
    strcpy(name, wksta.username);
    strlwr(name);
    return name;
  }


  /* ----- Novell NetWare ----- Get Connection Information E3(16) */

#pragma pack(1)
  static struct {
    unsigned short len;
    unsigned char func;
    unsigned char number;
  } NEAR gcireq;
  
  static struct {
    unsigned short len;
    unsigned long objectID;
    unsigned short objecttype;
    char objectname[48];
    unsigned char logintime[7];
    unsigned char reserved[39];
  } NEAR gcirep;
#pragma pack()

  /* Load Get Connection Number function code.   */
  r.x.ax = 0xDC00;
  int86x(0x21, &r, &r, &sregs);
  if (r.h.al > 0 && r.h.al <= 100) {
    /* If the connection number is in range 1-100,
     * invoke Get Connection Information to get the user name. */

    gcireq.len = sizeof(gcireq) - sizeof(gcireq.len);
    gcireq.func = 0x16;
    gcireq.number = r.h.al;
    gcirep.len = sizeof(gcirep) - sizeof(gcirep.len);

    r.h.ah = 0xE3;
    r.x.si = (unsigned short) &gcireq;
    r.x.di = (unsigned short) &gcirep;
    int86x(0x21, &r, &r, &sregs);
    if (r.h.al == 0) {
      strlwr(gcirep.objectname);
      return gcirep.objectname;
    }
  }


  /* Give up and use environment variables */
  const char * username = getenv("LOGNAME");
  if (username == NULL) {
    username = getenv("USER");
    if (username == NULL)
      username = "";
  }
  
  PAssert(*username != '\0', "Cannot determine user name, set LOGNAME.");
  return username;
}


// End Of File ///////////////////////////////////////////////////////////////
