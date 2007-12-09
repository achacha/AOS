print("======Script 3 executing======\r\nRequest header access\r\n")
print ("Host: " .. aos.getRequestHeader("Host") .. "\r\n");
print ("--START: All--\r\n" .. aos.getRequestHeader("_ALL") .. "--END: All----\r\n");
