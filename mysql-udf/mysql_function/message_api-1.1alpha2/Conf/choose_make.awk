{
    if($1 ~ /.*darwin.*/)  print "Make_darwin_so" 
    else print "Make_gcc_so"
}

