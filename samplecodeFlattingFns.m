{
    c = 1 + (2 * das()) + 1;

    c1();
    c1();
    c2();
    c2();
    
    let c1 = getCounter(0);
    let c2 = getCounter(1);

    let step = 1;

    let getCounter = (init){};
    
    let c = init;
    return (){
        c = c + step;
        return c;
    };

    let main = (){
        main();
        return a + b * 42;
    };
    
    main();

    let getCounter = (init){
        let c = init;
        return (){
            c = c + step;
            return c;
        };
    };
}