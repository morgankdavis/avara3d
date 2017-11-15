/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2016, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/
/** @file Default implementation of IOSystem using the standard C file functions */

#include "DefaultIOSystem.h"
#include "DefaultIOStream.h"
#include "StringComparison.h"

#include <assimp/DefaultLogger.hpp>
#include <assimp/ai_assert.h>
#include <stdlib.h>


#ifdef __unix__
#include <sys/param.h>
#include <stdlib.h>
#endif


#ifdef MINGW // added by Morgan
    #include <windows.h>
    #include <stdlib.h>
    #include <limits.h>
    #include <errno.h>
    #include <sys/stat.h>
#endif



using namespace Assimp;

// ------------------------------------------------------------------------------------------------
// Constructor.
DefaultIOSystem::DefaultIOSystem()
{
    // nothing to do here
}

// ------------------------------------------------------------------------------------------------
// Destructor.
DefaultIOSystem::~DefaultIOSystem()
{
    // nothing to do here
}

// ------------------------------------------------------------------------------------------------
// Tests for the existence of a file at the given path.
bool DefaultIOSystem::Exists( const char* pFile) const
{
    FILE* file = ::fopen( pFile, "rb");
    if( !file)
        return false;

    ::fclose( file);
    return true;
}

// ------------------------------------------------------------------------------------------------
// Open a new file with a given path.
IOStream* DefaultIOSystem::Open( const char* strFile, const char* strMode)
{
    ai_assert(NULL != strFile);
    ai_assert(NULL != strMode);

    FILE* file = ::fopen( strFile, strMode);
    if( NULL == file)
        return NULL;

    return new DefaultIOStream(file, (std::string) strFile);
}

// ------------------------------------------------------------------------------------------------
// Closes the given file and releases all resources associated with it.
void DefaultIOSystem::Close( IOStream* pFile)
{
    delete pFile;
}

// ------------------------------------------------------------------------------------------------
// Returns the operation specific directory separator
char DefaultIOSystem::getOsSeparator() const
{
#ifndef _WIN32
    return '/';
#else
    return '\\';
#endif
}

// ------------------------------------------------------------------------------------------------
// IOSystem default implementation (ComparePaths isn't a pure virtual function)
bool IOSystem::ComparePaths (const char* one, const char* second) const
{
    return !ASSIMP_stricmp(one,second);
}

// maximum path length
// XXX http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
#ifdef PATH_MAX
#   define PATHLIMIT PATH_MAX
#else
#   define PATHLIMIT 4096
#endif

// ------------------------------------------------------------------------------------------------
// Convert a relative path into an absolute path
#ifdef MINGW // added by Morgan
//inline void MakeAbsolutePath (const char* in, char* _out)
//{
//    strcpy(_out,in); // yikes...
//}


// custom realpath()

#define PATH_MAX PATHLIMIT
char *realpath(const char *path, char resolved_path[PATH_MAX])
{
    char *return_path = 0;

    if (path) //Else EINVAL
    {
        if (resolved_path)
        {
            return_path = resolved_path;
        }
        else
        {
            //Non standard extension that glibc uses
            return_path = (char*)malloc(PATH_MAX);
        }

        if (return_path) //Else EINVAL
        {
            //This is a Win32 API function similar to what realpath() is supposed to do
            size_t size = GetFullPathNameA(path, PATH_MAX, return_path, 0);

            //GetFullPathNameA() returns a size larger than buffer if buffer is too small
            if (size > PATH_MAX)
            {
                if (return_path != resolved_path) //Malloc'd buffer - Unstandard extension retry
                {
                    size_t new_size;

                    free(return_path);
                    return_path = (char*)malloc(size);

                    if (return_path)
                    {
                        new_size = GetFullPathNameA(path, size, return_path, 0); //Try again

                        if (new_size > size) //If it's still too large, we have a problem, don't try again
                        {
                            free(return_path);
                            return_path = 0;
                            errno = ENAMETOOLONG;
                        }
                        else
                        {
                            size = new_size;
                        }
                    }
                    else
                    {
                        //I wasn't sure what to return here, but the standard does say to return EINVAL
                        //if resolved_path is null, and in this case we couldn't malloc large enough buffer
                        errno = EINVAL;
                    }
                }
                else //resolved_path buffer isn't big enough
                {
                    return_path = 0;
                    errno = ENAMETOOLONG;
                }
            }

            //GetFullPathNameA() returns 0 if some path resolve problem occured
            if (!size)
            {
                if (return_path != resolved_path) //Malloc'd buffer
                {
                    free(return_path);
                }

                return_path = 0;

                //Convert MS errors into standard errors
                switch (GetLastError())
                {
                    case ERROR_FILE_NOT_FOUND:
                        errno = ENOENT;
                        break;

                    case ERROR_PATH_NOT_FOUND: case ERROR_INVALID_DRIVE:
                        errno = ENOTDIR;
                        break;

                    case ERROR_ACCESS_DENIED:
                        errno = EACCES;
                        break;

                    default: //Unknown Error
                        errno = EIO;
                        break;
                }
            }

            //If we get to here with a valid return_path, we're still doing good
            if (return_path)
            {
                struct stat stat_buffer;

                //Make sure path exists, stat() returns 0 on success
                if (stat(return_path, &stat_buffer))
                {
                    if (return_path != resolved_path)
                    {
                        free(return_path);
                    }

                    return_path = 0;
                    //stat() will set the correct errno for us
                }
                //else we succeeded!
            }
        }
        else
        {
            errno = EINVAL;
        }
    }
    else
    {
        errno = EINVAL;
    }

    return return_path;
}


inline void MakeAbsolutePath (const char* in, char* _out)
{
    ai_assert(in && _out);
    char* ret;

    ret = realpath(in, _out);

    if(!ret) {
        // preserve the input path, maybe someone else is able to fix
        // the path before it is accessed (e.g. our file system filter)
        DefaultLogger::get()->warn("Invalid path: "+std::string(in));
        strcpy(_out,in);
    }
}


#else
inline void MakeAbsolutePath (const char* in, char* _out)
{
    ai_assert(in && _out);
    char* ret;
#if defined( _MSC_VER ) || defined( __MINGW32__ )
    ret = ::_fullpath( _out, in, PATHLIMIT );
#else
    // use realpath
    ret = realpath(in, _out);
#endif
    if(!ret) {
        // preserve the input path, maybe someone else is able to fix
        // the path before it is accessed (e.g. our file system filter)
        DefaultLogger::get()->warn("Invalid path: "+std::string(in));
        strcpy(_out,in);
    }
}
#endif

// ------------------------------------------------------------------------------------------------
// DefaultIOSystem's more specialized implementation
bool DefaultIOSystem::ComparePaths (const char* one, const char* second) const
{
    // chances are quite good both paths are formatted identically,
    // so we can hopefully return here already
    if( !ASSIMP_stricmp(one,second) )
        return true;

    char temp1[PATHLIMIT];
    char temp2[PATHLIMIT];

    MakeAbsolutePath (one, temp1);
    MakeAbsolutePath (second, temp2);

    return !ASSIMP_stricmp(temp1,temp2);
}

// ------------------------------------------------------------------------------------------------
std::string DefaultIOSystem::fileName( const std::string &path )
{
    std::string ret = path;
    std::size_t last = ret.find_last_of("\\/");
    if (last != std::string::npos) ret = ret.substr(last + 1);
    return ret;
}

// ------------------------------------------------------------------------------------------------
std::string DefaultIOSystem::completeBaseName( const std::string &path )
{
    std::string ret = fileName(path);
    std::size_t pos = ret.find_last_of('.');
    if(pos != ret.npos) ret = ret.substr(0, pos);
    return ret;
}

// ------------------------------------------------------------------------------------------------
std::string DefaultIOSystem::absolutePath( const std::string &path )
{
    std::string ret = path;
    std::size_t last = ret.find_last_of("\\/");
    if (last != std::string::npos) ret = ret.substr(0, last);
    return ret;
}

// ------------------------------------------------------------------------------------------------

#undef PATHLIMIT
