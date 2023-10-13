import plotly.graph_objects as go

file1 = open('../FPS.profile', 'r')
Lines = file1.readlines()
 
TimeStamps = []
Values = []

count = 0
for line in Lines:
    count += 1
    TimeStamps.append(int(line[line.find("TimeStamp: ") + len("TimeStamp: ") : line.find(" |")]))
    Values.append(int(line[line.find("Value: ") + len("Value: ") : len(line)]))


fig = go.Figure(data=go.Scatter(
        x=TimeStamps,
        y=Values,
        error_y=dict(
            type='data',
            symmetric=False,
            array=[0.1, 0.2, 0.1, 0.1],
            arrayminus=[0.2, 0.4, 1, 0.2])
        ))
fig.show()
