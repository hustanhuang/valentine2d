setColor(102, 204, 255)

for i = 1, 6 do
    drawLine(
        256, 256,
        256+256*math.cos(math.pi*i/3),
        256+256*math.sin(math.pi*i/3)
    )
end
