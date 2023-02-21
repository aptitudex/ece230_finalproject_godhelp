import hashlib

def hash_file(input_file, **kwargs):
    m = hashlib.sha1()
    output_file = kwargs.get("outfile")


    with open(input_file) as input:
        data = input.read()
        m.update(bytes(data, 'utf-8'))


    if output_file == None:
        output_file = input_file+".hash"

    with open(output_file,'w+b') as output:
        output.write(m.digest())
    print(m.hexdigest())

    
    



    