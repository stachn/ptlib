/*
 * thread.h
 *
 * Thread of execution control class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */


///////////////////////////////////////////////////////////////////////////////
// PThread

#define PTHREAD_ID_FMT "%u"

  public:
    HANDLE GetHandle() const { return m_threadHandle; }
    void Win32AttachThreadInput();

  protected:
    PWin32Handle m_threadHandle;

  private:
    static UINT __stdcall MainFunction(void * thread);

#if defined(P_WIN_COM)
  public:
    bool CoInitialise();
  private:
    bool m_comInitialised;
#endif
  
// End Of File ///////////////////////////////////////////////////////////////
