
var fib = function(n) {
    if (n<2) {
        return 1;
    } else {
        return fib(n-2)+fib(n-1);
    }
}

var test = function(funct, max) {
    for (var i = 0; i<max; i++) {
        console.log(funct(i) + " ");
    }
}

test(fib(), prompt("Maximum "));
