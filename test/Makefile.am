#
#      $Id$
#
#########################################################################
#									#
#			   Copyright (C)  2003				#
#	     			Internet2				#
#			   All Rights Reserved				#
#									#
#########################################################################
#
#	File:		Makefile.am
#
#	Author:		Jeff Boote
#			Internet2
#
#	Date:		Thu Dec 18 14:28:42 MST 2003
#
#	Description:

INCLUDES	= $(I2UTILINCS)

TESTS			= sha1test hmac-sha1test pbkdf2test
check_PROGRAMS		= $(TESTS)
sha1test_SOURCES	= sha1test.c
sha1test_LDADD		= $(I2UTILLIBS) $(MALLOCDEBUGLIBS)
sha1test_DEPENDENCIES	= $(I2UTILLIBDEPS)

hmac_sha1test_SOURCES		= hmac-sha1test.c
hmac_sha1test_LDADD		= $(I2UTILLIBS) $(MALLOCDEBUGLIBS)
hmac_sha1test_DEPENDENCIES	= $(I2UTILLIBDEPS)

pbkdf2test_SOURCES		= pbkdf2test.c
pbkdf2test_LDADD		= $(I2UTILLIBS) $(MALLOCDEBUGLIBS)
pbkdf2test_DEPENDENCIES	= $(I2UTILLIBDEPS)
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
