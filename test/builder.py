text_file = open("sample.txt", "w")

for i in range(100):
    builderString = "<div %LOC_{0}_HIDDEN%>\n<label for=\"LOC_{1}_POS\">{2}:</label>\n<input type=\"number\" id=\"LOC_{3}_ROW\" name=\"LOC_{4}_ROW\" value=\"%LOC_{5}_ROW%\" min=\"0\" max=\"63\">\n<input type=\"number\" id=\"LOC_{6}_COL\" name=\"LOC_{7}_COL\" value=\"%LOC_{8}_COL%\" min=\"0\" max=\"63\"><br>\n</div>\n\n".format(i,i,i,i,i,i,i,i,i)
    text_file.write(builderString)
text_file.close()