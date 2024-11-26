"""#ser = serial.Serial("COM3", 115200)

#############################################################################
# 반복문
#for i in range(1, 10, 1):
#    print(ser.readline())
#############################################################################

#############################################################################
# 배열선언
var = [0,1,2,3]
#############################################################################

#############################################################################
# 출력1
var[0]
#############################################################################

#############################################################################
# 출력2
print(var)
#############################################################################

#############################################################################
# 진수 표현
bin(5)
hex(13)
#############################################################################

#############################################################################
# 클래스 타입
var = []
print(type(var))
str = ""
print(type(str))
tu = ()
print(type(tu))
ba = bytearray(b"hello world")
type(ba)
print(ba[0])
print(ba[2])
print(ba[3])
print(ba[5])
hex(ba[5])
b = bytes([])
print(type(b))
b = bytes([32,33,34])
print(b)

#############################################################################
# Class
var1 = [0,1,2,3]
var2 = [4,5,6,7]
var3 = ["hello","world","steve"]
print(var1)
print(var2)
print(var3)
    # list 안에 list가 들어갈 수 있다.
var = [var1, var2, var3]
print(var)
print("{0}Hello{1} world".format("나", "18"))
print("{who}는 누구입니까?".format(who="steve"))
who="steve2"
print("{who}는 누구입니까?")
value = 1,1,2,3,5,8,13,21
print(type(value))
print(value.count(1))
print(value.count(3))
l = [1,2]
l.append(3)
print(l)
l[3:] = [4] #append
print(l)
l.insert(2, 100)
print(l)
l.remove(100)
print(l)
val = l.pop(2)
print(l)
print(val)
del l[1]
print(l)
number = [9,8,7,6,5,4,3,2,1]
print(number)
number.sort()
print(number)
number.sort(reverse=True)
print(number)
number.reverse()
print(number)
number.reverse()
print(number)

a = number[::-1] # 자기 자신만 뒤집음
print(a)
print(number)

blist = [1,2,3,10,15,255]
the_bytes = bytes(blist)
print(the_bytes)
type_byte_array = bytearray(blist)
print(type_byte_array)
print(the_bytes[0])
print(the_bytes[5])
string = "10,20,30"
a1,a2,a3 = map(int, string.split(','))
print(string.split(','))
#############################################################################
"""
import serial
import matplotlib.pyplot as plt     # pyplot , animation 이라는 서브 모듈이 존재한다
import matplotlib.animation as ani
from collections import deque       #

ser = serial.Serial("COM3", 115200)
# return 이 두개이다 (python의 기능)
x, y = 3, 4 
x, *y = 3, 4, 5
print("x={x}")
print(y)

fig, ax = plt.subplots()
#data = deque(maxlen=720)
bar = ax.bar([0,30], [0,0], width=10)
ax.set_aspect("equal")
ax.set_ylim(-20,100)

def update_bar(frame):
    # 문자가 수신되어 있다면
    if ser.in_waiting > 0:
        line = ser.readline().decode("utf-8").rstrip()
        try:
            #value = float(line) # line을 float으로 파싱
            value1, value2 = map(float, line.split('\t'))
            bar[0].set_height(value1)
            bar[1].set_height(value2)
        except ValueError:
            pass
anim = ani.FuncAnimation(fig, update_bar, interval = 1, save_count=50)
plt.show()
ser.close()

