def process(i,ans,b,t):
    i=int(i)
    ans=int(ans)
    b=int(b)
    t-int(t)
    if(i<ans):
        b=i+1
    else:
        t=i-1
    return b,t

print("Welcome to the Number Guessing Game!")
ans = input("Please enter the target number:")
b=1
t=10
i=0
while(i != ans):
    i=input("Please enter a number between {} and {}:".format(b,t))
    b,t=process(i,ans,b,t)
print("Bingo!")