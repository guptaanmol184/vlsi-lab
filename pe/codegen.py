#for i in range(0, 16):
#    print('\tpriority_encoder_4bit s2pe_{}(s1v[{}:{}], s2po[{}], s2v[{}]);'.format(i,((i+1)*4)-1, i*4, i, i))

for i in range(0, 64):
    print('\tassign out[{}] = s2m[{}] & s1po[{}][0];'.format(i*4+0,i,i))
    print('\tassign out[{}] = s2m[{}] & s1po[{}][1];'.format(i*4+1,i,i))
    print('\tassign out[{}] = s2m[{}] & s1po[{}][2];'.format(i*4+2,i,i))
    print('\tassign out[{}] = s2m[{}] & s1po[{}][3];'.format(i*4+3,i,i))
    print()

