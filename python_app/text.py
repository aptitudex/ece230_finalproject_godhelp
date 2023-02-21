# define the input and output file names
input_file = 'NeverGonnaGiveYouUp.txt'
output_file = 'output.txt'

# open the input file for reading and the output file for writing
with open(input_file, 'r') as in_file, open(output_file, 'w') as out_file:
    # loop through each line in the input file
    for line in in_file:
        # add quotes to the beginning and end of the line
        new_line = '"' + line.strip() + '"\n'
        # write the updated line to the output file
        out_file.write(new_line)