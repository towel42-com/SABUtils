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

include( ${CMAKE_CURRENT_LIST_DIR}/CompilerSettings.cmake )

add_compile_options(
    "$<$<CONFIG:Debug>:-DQT_DEBUG>" 
    "$<$<CONFIG:Release>:-DQT_NO_DEBUG -DQT_NO_NDEBUG -DQT_NO_DEBUG_OUTPUT>" 
    "$<$<CONFIG:RelWithDebInfo>:-DQT_NO_DEBUG -DQT_NO_NDEBUG -DQT_NO_DEBUG_OUTPUT>" 
    "$<$<CONFIG:MinSizeRel>:-DQT_NO_DEBUG -DQT_NO_NDEBUG -DQT_NO_DEBUG_OUTPUT>" 
    )
add_definitions( -DQT_STRICT_ITERATORS )
add_definitions( -DQT_CC_WARNINGS -DQT_NO_WARNINGS )
