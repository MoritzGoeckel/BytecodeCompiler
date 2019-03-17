{
    let step = 1
    let getCounter = (init){
        let c = init;
        return (){
            c = c + step;
            return c;
        };
    };
    
    let c1 = getCounter(0);
    let c2 = getCounter(1);
    
    c1(); //1
    c1(); //2
    c2(); //2
    c2(); //3
}

TO

1. Make outer block main
2. Identify implicit parameters
3. Flat methods
4. Replace definition assigments with function pointers plus implicit parameters

All the data is preserved in the function pointers

let main_getCounter = (init) #step {
    let x = 90;
    let c = init;
    return main_getCounter_anon1 #c #step
};

let main_getCounter_anon1() #c #step {
    c = c + step;
    return c;
};

let main = (){
    let step = 1
    
    let c1 = main_getCounter(0) #step;
    let c2 = main_getCounter(1) #step;
    
    c1(); //1
    c1(); //2
    c2(); //2
    c2(); //3
}