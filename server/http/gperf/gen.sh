#!/bin/bash

# -D 去除重复的关键字，不加 -D 会生成很多空串
# -G：该选项将生成查找表并将其作为静态全局变量，而不是在查找函数内生成以隐藏该表(默认行为)

# -K：该选项允许用户选择关键字结构中的关键字组件的名称
# 即 HttpHeader结构中的第一个成员， 给个名字, 会影响到生成的 函数(in_word_set 或者自己指定)中如何获取该变量

# -L language-name：指示 gperf 使用指定的语言生成输出。
# C, C++, ANSI-C

# -N：该选项允许用户修改查找函数的名称。默认名为 in_word_set()。
# -t：该选项允许包含关键字结构。

# -Z 指定生成的类名字, 不指定则为PerfectHash


gperf -DG -K headerName -N isValidHttpHeader -L C++ -Z HttpCommomHeaderInternal -t HttpCommomHeadersHash.h.gperf > HttpCommomHeadersHash.cpp