var P = "";

var primes = function(n) {
  var count = 0;
  var flag = false;
  var i = 2;
  
  while (count < n) {
    for (j=i-1; j>1; j--) {
      if (i%j == 0)
        flag = true;
    }
    if (!flag) {
      P  += i+" ";
      count++;     
    }
    flag=false;
  	i++;
  }
}

var doPrimes = function() {
  var n = prompt("Enter number of primes to calculate: ");
  if (n) {
    primes(n);
    alert(P);
  }
}

