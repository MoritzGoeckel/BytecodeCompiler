{
    let c0 = 0;
    let c1 = 1;

    let fib = (n){
        if(n == 0){
            return c0;
        }

        if(n == 1){
            return c1;
        }

        return (fib(n - 1) + fib(n - 2));
    };

    print(fib(8));
}