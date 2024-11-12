import csv

with open('valid.csv', 'a', newline='') as f: #追加
    writer = csv.writer(f)
    for i in range(140,200):
        s = str(i)
        s = s.zfill(3)
        writer.writerow(["C:\\Users\\Marika Takahashi\\Desktop\\dataset\\valid\\itteki\\" + s + ".bmp"])
    for i in range(140, 200):
        s = str(i)
        s = s.zfill(3)
        writer.writerow(["C:\\Users\\Marika Takahashi\\Desktop\\dataset\\valid\\tadaima\\" + s + ".bmp"])
    for i in range(140, 200):
        s = str(i)
        s = s.zfill(3)
        writer.writerow(["C:\\Users\\Marika Takahashi\\Desktop\\dataset\\valid\\nene\\" + s + ".bmp"])