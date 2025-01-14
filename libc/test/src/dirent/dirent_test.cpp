//===-- Unittests for functions from POSIX dirent.h -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string_view.h"
#include "src/dirent/closedir.h"
#include "src/dirent/dirfd.h"
#include "src/dirent/opendir.h"
#include "src/dirent/readdir.h"

#include "utils/UnitTest/Test.h"

#include <dirent.h>
#include <errno.h>

using string_view = __llvm_libc::cpp::string_view;

TEST(LlvmLibcDirentTest, SimpleOpenAndRead) {
  ::DIR *dir = __llvm_libc::opendir("testdata");
  ASSERT_TRUE(dir != nullptr);
  // The file descriptors 0, 1 and 2 are reserved for standard streams.
  // So, the file descriptor for the newly opened directory should be
  // greater than 2.
  ASSERT_GT(__llvm_libc::dirfd(dir), 2);

  struct ::dirent *file1 = nullptr, *file2 = nullptr, *dir1 = nullptr,
                  *dir2 = nullptr;
  while (true) {
    struct ::dirent *d = __llvm_libc::readdir(dir);
    if (d == nullptr)
      break;
    if (string_view(&d->d_name[0]).equals("file1.txt"))
      file1 = d;
    if (string_view(&d->d_name[0]).equals("file2.txt"))
      file2 = d;
    if (string_view(&d->d_name[0]).equals("dir1"))
      dir1 = d;
    if (string_view(&d->d_name[0]).equals("dir2"))
      dir2 = d;
  }

  // Verify that we don't break out of the above loop in error.
  ASSERT_EQ(errno, 0);

  ASSERT_TRUE(file1 != nullptr);
  ASSERT_TRUE(file2 != nullptr);
  ASSERT_TRUE(dir1 != nullptr);
  ASSERT_TRUE(dir2 != nullptr);

  ASSERT_EQ(__llvm_libc::closedir(dir), 0);
}

TEST(LlvmLibcDirentTest, OpenNonExistentDir) {
  errno = 0;
  ::DIR *dir = __llvm_libc::opendir("___xyz123__.non_existent__");
  ASSERT_TRUE(dir == nullptr);
  ASSERT_EQ(errno, ENOENT);
  errno = 0;
}

TEST(LlvmLibcDirentTest, OpenFile) {
  errno = 0;
  ::DIR *dir = __llvm_libc::opendir("testdata/file1.txt");
  ASSERT_TRUE(dir == nullptr);
  ASSERT_EQ(errno, ENOTDIR);
  errno = 0;
}
