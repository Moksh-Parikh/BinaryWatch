# paste the list to be compressed here
list= [
         [1, 4],
         [4, 1],
         [2, 3],
         [3, 2],
         [2, 4],
         [4, 2],
         [3, 4],
         [4, 3]
        ]

for i in list:
    print(f'{(i[1] << 3) | i[0]},')
