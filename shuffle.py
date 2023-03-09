import subprocess
import re
import random
from itertools import permutations
import random

import matplotlib.pyplot as plt
import numpy as np
data = ['1', '2', '3']

# 測試 shuffle 次數
test_count = 1000000
input = "new\n"
for d in data:
    input+=f"it {d}\n"
for i in range(test_count):
    input += "shuffle\n"
input += "free\nquit\n"

# 取得 stdout 的 shuffle 結果
command='./qtest -v 3'
clist = command.split()
completedProcess = subprocess.run(clist, capture_output=True, text=True, input=input)
s = completedProcess.stdout
start = f"l = [{' '.join(data)}]"
startIdx = s.find(start) 
endIdx = s.find("l = NULL")
s = s[startIdx + len(start)+1 : endIdx]
Regex = re.compile(r'\[(.+)]')
result = Regex.findall(s)

def permute(nums):
    nums=list(permutations(nums,len(nums)))
    return nums

def chiSquared(observation, expectation):
    return ((observation - expectation) ** 2) / expectation 

# shuffle 的所有結果   
nums = []
for i in result:
    nums.append(i.split())

# 找出全部的排序可能
counterSet = {}
shuffle_array = data
s = permute(shuffle_array)

# 初始化 counterSet
for i in range(len(s)):
    w = ''.join(s[i])
    counterSet[w] = 0

# 計算每一種 shuffle 結果的數量
for num in nums:
    permutation = ''.join(num)
    counterSet[permutation] += 1
        
# 計算 chiSquare sum
expectation = test_count // len(s)
c = counterSet.values()
chiSquaredSum = 0
for i in c:
    chiSquaredSum += chiSquared(i, expectation)
print("Expectation: ", expectation)
print("Observation: ", counterSet)
print("chi square sum: ", chiSquaredSum)

permutations = counterSet.keys()
counts = counterSet.values()

x = np.arange(len(counts))
plt.bar(x, counts)
plt.xticks(x, permutations)
plt.xlabel('permutations')
plt.ylabel('counts')
plt.title('Shuffle result')
plt.show()