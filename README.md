# crashDemo 
windwos 下dump采集，以及符号服务的搭建

## 依赖环境
- cmake
- vs/vscode
- python >= 2.7
- [symstore.exe](https://chadaustin.me/2009/03/reporting-crashes-in-imvu-creating-your-very-own-symbol-server/)


## 生成项目
``` 
mkdir build
cd build
cmake ...
```

## 发布服务文件
```
python publish.py
```
