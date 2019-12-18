### PuzzleBrain - 基于DLX算法和MapReduce架构的分布式系统

目前已完成了手动MapReduce，可以将问题按层分解并保存为JSON结构（需要安装[rapidjson](https://github.com/Tencent/rapidjson)），供其他进程读入并做处理

```sh
# Master程序将问题分层，当某一层节点数>=16时，将缩小的问题保存为JSON文件
./sudoku -m -b16 -i ./hard.txt >./a.json

# 手动将a.json分解，供Slave程序并发处理，其中 '-r4' 是指输出前4个解
split -l1 a.json s.
./sudoku -r4 <./s/s.aa
./sudoku -r4 <./s/s.ab
...
```

后续Slave端可以将nc作为Socket服务器端读入，Master端直接往套接字写就可以完成初步的自动化。

