### PuzzleBrain - 基于DLX算法和MapReduce架构的分布式系统

目前已初步完成了MapReduce，可以将问题按层分解并保存为JSON结构（需要安装[rapidjson](https://github.com/Tencent/rapidjson)），供其他进程读入并做处理

通过ncat作为Socket服务器端读入JSON文件，并传递给Slave处理。

对于puzzler(Pentomino)程序：

```sh
# Slave (default maximum connections of ncat is 100)
ncat -4 -l 3001 -c ./run.sh --keep-open

# Master (8 or more processes concurrence)
./puzzler -m -b8 -s 127.0.0.1 -p 3001

# Observer
tail -f /tmp/puzzler.<pid>
```

Master(-m)進程通過select(2)獲知socket是否關閉，從而得知slave進程處理完成，由於沒
有使用libevent或Boost.Asio等庫，這是一個非常簡化的方案，沒有考慮到異常或錯誤處理
等。
