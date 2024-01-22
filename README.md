# fuse-ext4

Dependencies:

`sudo apt install libfuse3-dev`

Creation of test.ext4:

```
fallocate -l 15M test.ext4
mkfs.ext4 test.ext4
mkdir test
sudo mount test.ext4 test
```

We parse test.ext4 which contains an ext4 file system with the following files:

```
/a (directory)
/a/b (directory)
/a/b/{jkl} (empty file)
/lost+found
/with spaces (empty file)
/x (regular file with contents "hello world\n")
/y (empty file)
/z (empty file)
```
