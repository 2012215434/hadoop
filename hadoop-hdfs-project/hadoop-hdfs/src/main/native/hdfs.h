/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBHDFS_HDFS_H
#define LIBHDFS_HDFS_H

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <jni.h>

#ifndef O_RDONLY
#define O_RDONLY 1
#endif

#ifndef O_WRONLY 
#define O_WRONLY 2
#endif

#ifndef EINTERNAL
#define EINTERNAL 255 
#endif


/** All APIs set errno to meaningful values */
#ifdef __cplusplus
extern  "C" {
#endif

    /**
     * Some utility decls used in libhdfs.
     */
    struct hdfsBuilder;
    typedef int32_t   tSize; /// size of data for read/write io ops 
    typedef time_t    tTime; /// time type in seconds
    typedef int64_t   tOffset;/// offset within the file
    typedef uint16_t  tPort; /// port
    typedef enum tObjectKind {
        kObjectKindFile = 'F',
        kObjectKindDirectory = 'D',
    } tObjectKind;


    /**
     * The C reflection of org.apache.org.hadoop.FileSystem .
     */
    typedef void* hdfsFS;

    
    /**
     * The C equivalent of org.apache.org.hadoop.FSData(Input|Output)Stream .
     */
    enum hdfsStreamType
    {
        UNINITIALIZED = 0,
        INPUT = 1,
        OUTPUT = 2,
    };

    
    // Bit fields for hdfsFile_internal flags
    #define HDFS_FILE_SUPPORTS_DIRECT_READ (1<<0)

    /**
     * The 'file-handle' to a file in hdfs.
     */
    struct hdfsFile_internal {
        void* file;
        enum hdfsStreamType type;
        uint32_t flags;
    };
    typedef struct hdfsFile_internal* hdfsFile;
      
    /** 
     * hdfsConnectAsUser - Connect to a hdfs file system as a specific user
     * Connect to the hdfs.
     * @param nn   The NameNode.  See hdfsBuilderSetNameNode for details.
     * @param port The port on which the server is listening.
     * @param user the user name (this is hadoop domain user). Or NULL is equivelant to hhdfsConnect(host, port)
     * @return Returns a handle to the filesystem or NULL on error.
     * @deprecated Use hdfsBuilderConnect instead. 
     */
     hdfsFS hdfsConnectAsUser(const char* nn, tPort port, const char *user);


    /** 
     * hdfsConnect - Connect to a hdfs file system.
     * Connect to the hdfs.
     * @param nn   The NameNode.  See hdfsBuilderSetNameNode for details.
     * @param port The port on which the server is listening.
     * @return Returns a handle to the filesystem or NULL on error.
     * @deprecated Use hdfsBuilderConnect instead. 
     */
     hdfsFS hdfsConnect(const char* nn, tPort port);

    /** 
     * hdfsConnect - Connect to an hdfs file system.
     *
     * Forces a new instance to be created
     *
     * @param nn     The NameNode.  See hdfsBuilderSetNameNode for details.
     * @param port   The port on which the server is listening.
     * @param user   The user name to use when connecting
     * @return       Returns a handle to the filesystem or NULL on error.
     * @deprecated   Use hdfsBuilderConnect instead. 
     */
     hdfsFS hdfsConnectAsUserNewInstance(const char* nn, tPort port, const char *user );

    /** 
     * hdfsConnect - Connect to an hdfs file system.
     *
     * Forces a new instance to be created
     *
     * @param nn     The NameNode.  See hdfsBuilderSetNameNode for details.
     * @param port   The port on which the server is listening.
     * @return       Returns a handle to the filesystem or NULL on error.
     * @deprecated   Use hdfsBuilderConnect instead. 
     */
     hdfsFS hdfsConnectNewInstance(const char* nn, tPort port);

    /** 
     * Connect to HDFS using the parameters defined by the builder.
     *
     * The HDFS builder will be freed, whether or not the connection was
     * successful.
     *
     * Every successful call to hdfsBuilderConnect should be matched with a call
     * to hdfsDisconnect, when the hdfsFS is no longer needed.
     *
     * @param bld    The HDFS builder
     * @return       Returns a handle to the filesystem, or NULL on error.
     */
     hdfsFS hdfsBuilderConnect(struct hdfsBuilder *bld);

    /**
     * Create an HDFS builder.
     *
     * @return The HDFS builder, or NULL on error.
     */
    struct hdfsBuilder *hdfsNewBuilder(void);

    /**
     * Force the builder to always create a new instance of the FileSystem,
     * rather than possibly finding one in the cache.
     *
     * @param bld The HDFS builder
     */
    void hdfsBuilderSetForceNewInstance(struct hdfsBuilder *bld);

    /**
     * Set the HDFS NameNode to connect to.
     *
     * @param bld  The HDFS builder
     * @param nn   The NameNode to use.
     *             If the string given is 'default', the default NameNode
     *             configuration will be used (from the XML configuration files)
     *             If NULL is given, a LocalFileSystem will be created.
     *             Otherwise, the string will be interpreted as a hostname or IP
     *             address.
     */
    void hdfsBuilderSetNameNode(struct hdfsBuilder *bld, const char *nn);

    /**
     * Set the port of the HDFS NameNode to connect to.
     *
     * @param bld The HDFS builder
     * @param port The port.
     */
    void hdfsBuilderSetNameNodePort(struct hdfsBuilder *bld, tPort port);

    /**
     * Set the username to use when connecting to the HDFS cluster.
     *
     * @param bld The HDFS builder
     * @param userName The user name.  The string will be shallow-copied.
     */
    void hdfsBuilderSetUserName(struct hdfsBuilder *bld, const char *userName);

    /**
     * Set the path to the Kerberos ticket cache to use when connecting to
     * the HDFS cluster.
     *
     * @param bld The HDFS builder
     * @param kerbTicketCachePath The Kerberos ticket cache path.  The string
     *                            will be shallow-copied.
     */
    void hdfsBuilderSetKerbTicketCachePath(struct hdfsBuilder *bld,
                                   const char *kerbTicketCachePath);

    /**
     * Free an HDFS builder.
     *
     * It is normally not necessary to call this function since
     * hdfsBuilderConnect frees the builder.
     *
     * @param bld The HDFS builder
     */
    void hdfsFreeBuilder(struct hdfsBuilder *bld);

    /**
     * Get a configuration string.
     *
     * @param key      The key to find
     * @param val      (out param) The value.  This will be NULL if the
     *                 key isn't found.  You must free this string with
     *                 hdfsConfFree.
     *
     * @return         0 on success; nonzero error code otherwise.
     *                 Failure to find the key is not an error.
     */
    int hdfsConfGet(const char *key, char **val);

    /**
     * Free a configuration string found with hdfsConfGet. 
     *
     * @param val      A configuration string obtained from hdfsConfGet
     */
    void hdfsConfFree(char *val);

    /** 
     * hdfsDisconnect - Disconnect from the hdfs file system.
     * Disconnect from hdfs.
     * @param fs The configured filesystem handle.
     * @return Returns 0 on success, -1 on error.  
     */
    int hdfsDisconnect(hdfsFS fs);
        

    /** 
     * hdfsOpenFile - Open a hdfs file in given mode.
     * @param fs The configured filesystem handle.
     * @param path The full path to the file.
     * @param flags - an | of bits/fcntl.h file flags - supported flags are O_RDONLY, O_WRONLY (meaning create or overwrite i.e., implies O_TRUNCAT), 
     * O_WRONLY|O_APPEND. Other flags are generally ignored other than (O_RDWR || (O_EXCL & O_CREAT)) which return NULL and set errno equal ENOTSUP.
     * @param bufferSize Size of buffer for read/write - pass 0 if you want
     * to use the default configured values.
     * @param replication Block replication - pass 0 if you want to use
     * the default configured values.
     * @param blocksize Size of block - pass 0 if you want to use the
     * default configured values.
     * @return Returns the handle to the open file or NULL on error.
     */
    hdfsFile hdfsOpenFile(hdfsFS fs, const char* path, int flags,
                          int bufferSize, short replication, tSize blocksize);


    /** 
     * hdfsCloseFile - Close an open file. 
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @return Returns 0 on success, -1 on error.  
     */
    int hdfsCloseFile(hdfsFS fs, hdfsFile file);


    /** 
     * hdfsExists - Checks if a given path exsits on the filesystem 
     * @param fs The configured filesystem handle.
     * @param path The path to look for
     * @return Returns 0 on success, -1 on error.  
     */
    int hdfsExists(hdfsFS fs, const char *path);


    /** 
     * hdfsSeek - Seek to given offset in file. 
     * This works only for files opened in read-only mode. 
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @param desiredPos Offset into the file to seek into.
     * @return Returns 0 on success, -1 on error.  
     */
    int hdfsSeek(hdfsFS fs, hdfsFile file, tOffset desiredPos); 


    /** 
     * hdfsTell - Get the current offset in the file, in bytes.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @return Current offset, -1 on error.
     */
    tOffset hdfsTell(hdfsFS fs, hdfsFile file);


    /** 
     * hdfsRead - Read data from an open file.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @param buffer The buffer to copy read bytes into.
     * @param length The length of the buffer.
     * @return Returns the number of bytes actually read, possibly less
     * than than length;-1 on error.
     */
    tSize hdfsRead(hdfsFS fs, hdfsFile file, void* buffer, tSize length);

    /** 
     * hdfsPread - Positional read of data from an open file.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @param position Position from which to read
     * @param buffer The buffer to copy read bytes into.
     * @param length The length of the buffer.
     * @return Returns the number of bytes actually read, possibly less than
     * than length;-1 on error.
     */
    tSize hdfsPread(hdfsFS fs, hdfsFile file, tOffset position,
                    void* buffer, tSize length);


    /** 
     * hdfsWrite - Write data into an open file.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @param buffer The data.
     * @param length The no. of bytes to write. 
     * @return Returns the number of bytes written, -1 on error.
     */
    tSize hdfsWrite(hdfsFS fs, hdfsFile file, const void* buffer,
                    tSize length);


    /** 
     * hdfsWrite - Flush the data. 
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsFlush(hdfsFS fs, hdfsFile file);


    /**
     * hdfsHFlush - Flush out the data in client's user buffer. After the
     * return of this call, new readers will see the data.
     * @param fs configured filesystem handle
     * @param file file handle
     * @return 0 on success, -1 on error and sets errno
     */
    int hdfsHFlush(hdfsFS fs, hdfsFile file);


    /**
     * hdfsAvailable - Number of bytes that can be read from this
     * input stream without blocking.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @return Returns available bytes; -1 on error. 
     */
    int hdfsAvailable(hdfsFS fs, hdfsFile file);


    /**
     * hdfsCopy - Copy file from one filesystem to another.
     * @param srcFS The handle to source filesystem.
     * @param src The path of source file. 
     * @param dstFS The handle to destination filesystem.
     * @param dst The path of destination file. 
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsCopy(hdfsFS srcFS, const char* src, hdfsFS dstFS, const char* dst);


    /**
     * hdfsMove - Move file from one filesystem to another.
     * @param srcFS The handle to source filesystem.
     * @param src The path of source file. 
     * @param dstFS The handle to destination filesystem.
     * @param dst The path of destination file. 
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsMove(hdfsFS srcFS, const char* src, hdfsFS dstFS, const char* dst);


    /**
     * hdfsDelete - Delete file. 
     * @param fs The configured filesystem handle.
     * @param path The path of the file. 
     * @param recursive if path is a directory and set to 
     * non-zero, the directory is deleted else throws an exception. In
     * case of a file the recursive argument is irrelevant.
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsDelete(hdfsFS fs, const char* path, int recursive);

    /**
     * hdfsRename - Rename file. 
     * @param fs The configured filesystem handle.
     * @param oldPath The path of the source file. 
     * @param newPath The path of the destination file. 
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsRename(hdfsFS fs, const char* oldPath, const char* newPath);


    /** 
     * hdfsGetWorkingDirectory - Get the current working directory for
     * the given filesystem.
     * @param fs The configured filesystem handle.
     * @param buffer The user-buffer to copy path of cwd into. 
     * @param bufferSize The length of user-buffer.
     * @return Returns buffer, NULL on error.
     */
    char* hdfsGetWorkingDirectory(hdfsFS fs, char *buffer, size_t bufferSize);


    /** 
     * hdfsSetWorkingDirectory - Set the working directory. All relative
     * paths will be resolved relative to it.
     * @param fs The configured filesystem handle.
     * @param path The path of the new 'cwd'. 
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsSetWorkingDirectory(hdfsFS fs, const char* path);


    /** 
     * hdfsCreateDirectory - Make the given file and all non-existent
     * parents into directories.
     * @param fs The configured filesystem handle.
     * @param path The path of the directory. 
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsCreateDirectory(hdfsFS fs, const char* path);


    /** 
     * hdfsSetReplication - Set the replication of the specified
     * file to the supplied value
     * @param fs The configured filesystem handle.
     * @param path The path of the file. 
     * @return Returns 0 on success, -1 on error. 
     */
    int hdfsSetReplication(hdfsFS fs, const char* path, int16_t replication);


    /** 
     * hdfsFileInfo - Information about a file/directory.
     */
    typedef struct  {
        tObjectKind mKind;   /* file or directory */
        char *mName;         /* the name of the file */
        tTime mLastMod;      /* the last modification time for the file in seconds */
        tOffset mSize;       /* the size of the file in bytes */
        short mReplication;    /* the count of replicas */
        tOffset mBlockSize;  /* the block size for the file */
        char *mOwner;        /* the owner of the file */
        char *mGroup;        /* the group associated with the file */
        short mPermissions;  /* the permissions associated with the file */
        tTime mLastAccess;    /* the last access time for the file in seconds */
    } hdfsFileInfo;


    /** 
     * hdfsListDirectory - Get list of files/directories for a given
     * directory-path. hdfsFreeFileInfo should be called to deallocate memory. 
     * @param fs The configured filesystem handle.
     * @param path The path of the directory. 
     * @param numEntries Set to the number of files/directories in path.
     * @return Returns a dynamically-allocated array of hdfsFileInfo
     * objects; NULL on error.
     */
    hdfsFileInfo *hdfsListDirectory(hdfsFS fs, const char* path,
                                    int *numEntries);


    /** 
     * hdfsGetPathInfo - Get information about a path as a (dynamically
     * allocated) single hdfsFileInfo struct. hdfsFreeFileInfo should be
     * called when the pointer is no longer needed.
     * @param fs The configured filesystem handle.
     * @param path The path of the file. 
     * @return Returns a dynamically-allocated hdfsFileInfo object;
     * NULL on error.
     */
    hdfsFileInfo *hdfsGetPathInfo(hdfsFS fs, const char* path);


    /** 
     * hdfsFreeFileInfo - Free up the hdfsFileInfo array (including fields) 
     * @param hdfsFileInfo The array of dynamically-allocated hdfsFileInfo
     * objects.
     * @param numEntries The size of the array.
     */
    void hdfsFreeFileInfo(hdfsFileInfo *hdfsFileInfo, int numEntries);


    /** 
     * hdfsGetHosts - Get hostnames where a particular block (determined by
     * pos & blocksize) of a file is stored. The last element in the array
     * is NULL. Due to replication, a single block could be present on
     * multiple hosts.
     * @param fs The configured filesystem handle.
     * @param path The path of the file. 
     * @param start The start of the block.
     * @param length The length of the block.
     * @return Returns a dynamically-allocated 2-d array of blocks-hosts;
     * NULL on error.
     */
    char*** hdfsGetHosts(hdfsFS fs, const char* path, 
            tOffset start, tOffset length);


    /** 
     * hdfsFreeHosts - Free up the structure returned by hdfsGetHosts
     * @param hdfsFileInfo The array of dynamically-allocated hdfsFileInfo
     * objects.
     * @param numEntries The size of the array.
     */
    void hdfsFreeHosts(char ***blockHosts);


    /** 
     * hdfsGetDefaultBlockSize - Get the optimum blocksize.
     * @param fs The configured filesystem handle.
     * @return Returns the blocksize; -1 on error. 
     */
    tOffset hdfsGetDefaultBlockSize(hdfsFS fs);


    /** 
     * hdfsGetCapacity - Return the raw capacity of the filesystem.  
     * @param fs The configured filesystem handle.
     * @return Returns the raw-capacity; -1 on error. 
     */
    tOffset hdfsGetCapacity(hdfsFS fs);


    /** 
     * hdfsGetUsed - Return the total raw size of all files in the filesystem.
     * @param fs The configured filesystem handle.
     * @return Returns the total-size; -1 on error. 
     */
    tOffset hdfsGetUsed(hdfsFS fs);

    /** 
     * hdfsChown 
     * @param fs The configured filesystem handle.
     * @param path the path to the file or directory
     * @param owner this is a string in Hadoop land. Set to null or "" if only setting group
     * @param group  this is a string in Hadoop land. Set to null or "" if only setting user
     * @return 0 on success else -1
     */
    int hdfsChown(hdfsFS fs, const char* path, const char *owner, const char *group);

    /** 
     * hdfsChmod
     * @param fs The configured filesystem handle.
     * @param path the path to the file or directory
     * @param mode the bitmask to set it to
     * @return 0 on success else -1
     */
      int hdfsChmod(hdfsFS fs, const char* path, short mode);

    /** 
     * hdfsUtime
     * @param fs The configured filesystem handle.
     * @param path the path to the file or directory
     * @param mtime new modification time or -1 for no change
     * @param atime new access time or -1 for no change
     * @return 0 on success else -1
     */
    int hdfsUtime(hdfsFS fs, const char* path, tTime mtime, tTime atime);
    
#ifdef __cplusplus
}
#endif

#endif /*LIBHDFS_HDFS_H*/

/**
 * vim: ts=4: sw=4: et
 */
