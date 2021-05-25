# How to build
1. install cmake



2. build libevent

```
cd deps
git submodule add https://github.com/libevent/libevent
cd libevent
git checkout release-2.1.12-stable
mkdir build
cd build
cmake .. -DEVENT__DISABLE_OPENSSL=ON
make
sudo make install
```

3. build google test
```
cd deps
git submodule add https://github.com/google/googletest
cd googletest
mkdir build
cd build
cmake ..
make
sudo make install
```

4. build miniob

```shell
cd `project home`
mkdir build
cd build
cmake ..
make
```