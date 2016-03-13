
var seq = "";

var doFib = function() {
    seq = "1 ";
    var ans = prompt("Maximum number of values of the sequence to calculate:");
    if (ans) {
        fib(ans);
        alert(seq);
    }

};

var fib = function(numMax){
    for(i=0,j=1,k=0; k<numMax-1;i=j,j=x,k++ ){
        x=i+j;
        seq+=x+" ";
    }

}


