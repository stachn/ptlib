#---------------------------------------------
# Description: PTLib cmake-able build system
#
#---------------------------------------------
cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME "ptlib")
project(${PROJECT_NAME} C CXX)

file(GLOB PT_COMMON_SRC     ptlib/common/*.c*)
file(GLOB PT_REGEX_SRC      ptlib/common/regex/reg*.c)
file(GLOB PT_PLATFORM_SRC   ptlib/msos/*.cxx)

set(PT_COMPONENTS_SRC
    #asnber.cxx
    ptclib/asner.cxx
    #asnper.cxx
    #asnxer.cxx
    ptclib/cli.cxx
    ptclib/cypher.cxx
    ptclib/delaychan.cxx
    ptclib/dtmf.cxx
    ptclib/enum.cxx
    ptclib/ftp.cxx
    ptclib/ftpclnt.cxx
    ptclib/ftpsrvr.cxx
    ptclib/gstreamer.cxx
    ptclib/guid.cxx
    ptclib/html.cxx
    ptclib/http.cxx
    ptclib/httpclnt.cxx
    ptclib/httpform.cxx
    ptclib/httpsrvr.cxx
    ptclib/httpsvc.cxx
    ptclib/inetmail.cxx
    ptclib/inetprot.cxx
    ptclib/ipacl.cxx
    ptclib/jscript.cxx
    ptclib/lua.cxx
    ptclib/memfile.cxx
    ptclib/modem.cxx
    ptclib/pasn.cxx
    ptclib/pdns.cxx
    ptclib/pffvdev.cxx
    ptclib/pils.cxx
    ptclib/pldap.cxx
    ptclib/pnat.cxx
    ptclib/podbc.cxx
    #    ptclib/portaudio.cxx
    ptclib/psasl.cxx
    ptclib/psnmp.cxx
    ptclib/psoap.cxx
    ptclib/psockbun.cxx
    ptclib/pssl.cxx
    ptclib/pstun.cxx
    ptclib/pstunsrvr.cxx
    ptclib/ptts.cxx
    ptclib/pvfiledev.cxx
    ptclib/pvidfile.cxx
    ptclib/pwavfile.cxx
    ptclib/pwavfiledev.cxx
    ptclib/pxml.cxx
    ptclib/pxmlrpc.cxx
    ptclib/pxmlrpcs.cxx
    ptclib/qchannel.cxx
    ptclib/random.cxx
    ptclib/rfc1155.cxx
    ptclib/script.cxx
    ptclib/shttpsvc.cxx
    ptclib/snmp.cxx
    ptclib/snmpclnt.cxx
    ptclib/snmpserv.cxx
    ptclib/socks.cxx
    ptclib/spooldir.cxx
    ptclib/ssdp.cxx
    ptclib/telnet.cxx
    ptclib/threadpool.cxx
    ptclib/tonedev.cxx
    ptclib/url.cxx
    ptclib/vartype.cxx
    ptclib/vcard.cxx
    ptclib/vsdl.cxx
    ptclib/vxml.cxx
    ptclib/xmpp.cxx
    ptclib/xmpp_c2s.cxx
    ptclib/xmpp_muc.cxx
    ptclib/xmpp_roster.cxx
)

source_group(common FILES ${PT_COMMON_SRC})
source_group(regex FILES ${PT_REGEX_SRC})
source_group(platform FILES ${PT_PLATFORM_SRC})
source_group(components FILES ${PT_COMPONENTS_SRC})

include_directories(${PTLIB_INCLUDE_PATH})
include_directories(${PTLIB_SRC_ROOT})

# internal inculdes
include_directories(${PROJECT_SOURCE_DIR}/ptlib/common/regex)

add_library(${ptlib}
   ${PT_COMMON_SRC}
   ${PT_REGEX_SRC}
   ${PT_PLATFORM_SRC}
   ${PT_COMPONENTS_SRC}
   )

set_target_properties(${ptlib} PROPERTIES DEBUG_POSTFIX "d")
