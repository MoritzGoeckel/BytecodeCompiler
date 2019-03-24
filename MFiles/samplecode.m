{
    let a = 3;

    retNine = (b){
        print(90);
        return 9 + 30;
    };

    doSomething = (x, y){
        print(x);
        print(y());
    };

    doSomething(a, retNine);
}