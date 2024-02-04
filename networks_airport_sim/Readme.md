# CS342 Assgn 4, Group- CS18
Ridhiman Dhindsa, 210101088  
Dhruv Patel, 210101075  
Harsh Katara, 210101045  

## Q1
Compile and run:  
`g++ que1.cpp -o que1`  
``./que1``  


## Q2
### Single server, infinite buffer
Compile and run:    
`g++ q2_singleserver.cpp -o q2_singleserver`  
`./q2_singleserver 5 1`  
`./q2_singleserver 1 2`  
`./q2_singleserver 1 1`  

### Single server, finite buffer (k)
Compile and run:  
`g++ q2_finite_buffer.cpp -o q2_finite_buffer`
`./q2_finite_buffer 5 1 10`  
`./q2_finite_buffer 1 2 10`  
`./q2_finite_buffer 1 1 10`  

`./q2_finite_buffer 9 1 20`  
`./q2_finite_buffer 9 1 100`  

### Multi server with no buffers and finite queue (k)
Compile and run:  
`g++ q2_multiserver.cpp -o q2_multiserver`  
`./q2_multiserver 5 1 10 2`

### Multi server (m), multi buffer (m)
Compile and run:  
`g++ q2_multibuffer.cpp -o q2_multibuffer`  
`./q2_multibuffer 5 1 3 2`