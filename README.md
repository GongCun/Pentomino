### PuzzleBrain - 基于DLX算法和MapReduce架构的分布式系统

目前已初步完成了MapReduce，可以将问题按层分解并保存为JSON结构（需要安装[rapidjson](https://github.com/Tencent/rapidjson)），供其他进程读入并做处理

```sh
# Master程序将问题分层，当某一层节点数>=16时，将缩小的问题保存为JSON文件
./sudoku -m -b16 -i ./hard.txt >./a.json

# 手动将a.json分解，供Slave程序并发处理，其中 '-r4' 是指输出前4个解
split -d -1 a.json s.
./sudoku -r4 <./s.00
./sudoku -r4 <./s.01
...
```

通过ncat作为Socket服务器端读入JSON文件，并传递给Slave处理：

```sh
# Prepare the script
cat ./run.sh
#!/bin/sh
dir=`dirname $0`
dir=`(cd $dir; pwd -P)`
cd $dir
./sudoku >/tmp/sudoku.$$

# Slave (default maximum connections of ncat is 100)
ncat -4 -l 3001 -c ./run.sh --keep-open --recv-only

# Master (4 processes concurrence)
./sudoku -m -b4 -i ./hard.txt -s 127.0.0.1 -p 3001

# Observer
tail -f /tmp/sudoku.<pid>
```

