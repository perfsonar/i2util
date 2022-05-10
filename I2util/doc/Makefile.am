#
#      $Id$
#
#########################################################################
#									#
#			   Copyright (C)  2004				#
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
#	Date:		Tue Feb 17 11:08:20 MST 2004
#
#	Description:

dist_man1_MANS = aespasswd.man pfstore.man

SUFFIXES = .man

if HAVE_MAN2HTML
SUFFIXES += .man.html
.man.man.html:
	man2html -r $< | grep -v Content-type | sed -e 's/..\/man[0-9]\///g' | sed -e 's/\.[0-9]\.html/.man.html/g' > $@

EXTRA_DIST = aespasswd.man.html pfstore.man.html
endif
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
