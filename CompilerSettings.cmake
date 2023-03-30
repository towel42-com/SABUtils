# The MIT License( MIT )
#
# Copyright( c ) 2020-2021 Scott Aron Bloom
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files( the "Software" ), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

IF(CMAKE_SIZEOF_VOID_P EQUAL 4) #32 bit
    SET( BITSIZE 32 )
ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 8) #64 bit
    SET( BITSIZE 64 )
ELSE () 
    MESSAGE( STATUS "Unknown Bitsize - CMAKE_SIZEOF_VOID_P not set to 4 or 8" )
    MESSAGE( STATUS "-DCMAKE_SIZEOF_VOID_P=4 for 32 bit" )
    MESSAGE( FATAL_ERROR "-DCMAKE_SIZEOF_VOID_P=8 for 64 bit" )
ENDIF() 

IF(NOT WIN32)
    IF( NOT WARN_ALL )
        MESSAGE( STATUS  "Warning all enabled - ${CMAKE_PROJECT_NAME}" )
    ENDIF()
ENDIF()

IF(${NO_EXTENSIVE_WARNINGS})
    MESSAGE( WARNING "Extensive warnings disabled - ${PROJECT_NAME}" )
ENDIF()

IF( NO_WARNING_AS_ERROR)
    MESSAGE( WARNING "Warning as an Error disabled - ${PROJECT_NAME}" )
ENDIF()

IF(${NO_EXTENSIVE_WARNINGS})
    MESSAGE( WARNING "Extensive warnings disabled - ${PROJECT_NAME}" )
ENDIF()


add_compile_definitions( 
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS  
    UNICODE 
    $<$<BOOL:SABUTILS_DLL>:SABUTILS_DLL>
    $<$<BOOL:SAB_DEBUG_TRACE>:SAB_DEBUG_TRACE>

    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:_DEBUG>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:NDEBUG>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:NDEBUG>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:MinSizeRel>>:NDEBUG>

    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_DEPRECATE>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_NONSTDC_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<EQUAL:${BITSIZE},64>>:_AMD64_>
)

add_link_options(
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<EQUAL:${BITSIZE},64>>:/STACK:18388608>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<EQUAL:${BITSIZE},64>>:/HIGHENTROPYVA:NO>
)

add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
    $<$<CXX_COMPILER_ID:MSVC>:/w34700> 
    $<$<CXX_COMPILER_ID:MSVC>:/w34701> 
    $<$<CXX_COMPILER_ID:MSVC>:/w34715> 
    $<$<CXX_COMPILER_ID:MSVC>:/w34716> 
    $<$<CXX_COMPILER_ID:MSVC>:/w34717> 
    $<$<CXX_COMPILER_ID:MSVC>:/w34062> 
    $<$<CXX_COMPILER_ID:MSVC>:/w44800> 
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251> 
    $<$<CXX_COMPILER_ID:MSVC>:/wd4231> 
    $<$<CXX_COMPILER_ID:MSVC>:/wd4503>

    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/MDd>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/ZI>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/W3>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Od>

    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/MD>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Zi>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Ob1>

    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/MD>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/Zi>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/O2>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RelWithDebInfo>>:/Ob1>

    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:MinSizeRel>>:/MD>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:MinSizeRel>>:/Zi>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:MinSizeRel>>:/O2>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:MinSizeRel>>:/Ob1>
    
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<BOOL:${NO_WARNING_AS_ERROR}>>>:/WX>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<BOOL:${NO_WARNING_AS_ERROR}>>>:/w34100>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<BOOL:${WARN_ALL}>>:/W4>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<EQUAL:${BITSIZE},32>,$<VERSION_GREATER_EQUAL:${MSVC_VERSION},1800>>:/SAFESEH:NO>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<EQUAL:${BITSIZE},64>>:/bigobj>

    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Waddress>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wchar-subscripts>

    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wcomment>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wformat>

    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wmaybe-uninitialized>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wmissing-braces>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wnonnull>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wparentheses>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wreturn-type>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wsequence-point>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wsign-compare> # big changes

    # we dont use -fstrict aliasing or -fstrict-overflow the following 2 are worthless
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wstrict-aliasing>
    # $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wstrict-overflow=1>

    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wswitch>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wtrigraphs>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wuninitialized>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wunknown-pragmas> # have to figure out a way with windows
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wunused-function>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wunused-label>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wunused-value>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wunused-variable>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wvolatile-register-var>

    #the following are form -Wextra
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wclobbered>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wempty-body>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wignored-qualifiers>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wmissing-field-initializers>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wtype-limits>
    # $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wshift-negative-value> #not available on our compiler
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wunused-parameter>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wunused-but-set-parameter>

    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wno-return-local-addr>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wreorder>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wc++11-compat>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Werror=suggest-override>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_EXTENSIVE_WARNINGS}>>>:-Wc++14-compat> # not available on our compiler

    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<NOT:$<BOOL:${NO_WARNING_AS_ERROR}>>>:-Werror>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<BOOL:${WARN_ALL}$>>:-Wall>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fPIC>
)

