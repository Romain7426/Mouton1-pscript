begin 
  menu;
end; 


begin 
  Writeln('Hello World!','Hello World!','Hello World!'); 
end; 





function factorial(n	: integer):integer;
begin
   //writeln('FACT CALLED - n = ', n); 
   if n <= 0 then retval := 1;
   else retval := n * rec(n - 1); 
   //else retval := n * factorial(n - 1); 
end; 

begin 
  Writeln('Factorial(0)  = ', factorial(0)); 
  Writeln('Factorial(1)  = ', factorial(1)); 
  Writeln('Factorial(2)  = ', factorial(2)); 
  Writeln('Factorial(3)  = ', factorial(3)); 
  Writeln('Factorial(10) = ', factorial(10)); 
end; 





function factorial_fake(n	: integer):integer
begin 
   retval := 42; 
end; 

function factorial_fake_bis(n	: integer;;;;;):integer;;;;;;;;
begin 
   retval := 42; 
end; 
begin
   writeln('FACTORIAL_FAKE defined'); 
end; 

begin
   writeln('FACT_REC defined'); 
end; 

begin
  Writeln('Raw number = ', 10); 
end; 

begin
  Writeln('factorial_fake(10) = ', factorial_fake(10)); 
  Writeln('factorial_fake(10) = ', factorial_fake(10)); 
end; 
begin
  Writeln('factorial_fake(10) = ', factorial_fake(10)); 
  Writeln('factorial_fake(10) = ', factorial_fake(10)); 
end; 




// ------------------------------------------------------------------- 

procedure print_seq(	n :  integer); 
  var i,j,k :  integer; 
begin 
   begin 
      writeln('print_seq! BEGIN'); 
   end; 
   for i := 1 to 2*n do begin 
      writeln('  --> i = ', i); 
   end; 
/*/
/*
***
* ////
*/
   
   writeln('print_seq! END'); 
end; 


begin 
   print_seq(10); 
end; 


// ------------------------------------------------------------------- 

function factorial2(n	: integer):integer;
begin
   if n <= 0 then retval := 1; 
   //else retval := n * factorial2(n - 1); 
   else retval := n * rec(n - 1); 
end; 

function factorial3(n	: integer):integer;
begin
   if n <= 0 then retval := 1; 
   else retval := n * rec(n - 1); 
   //else retval := n * factorial3(n - 1);  
end; 

fonction factorial_with_for(n : integer): integer; 
var i :  integer; 
begin
   retval := 1; 
   for i := 2 to n do 
      retval := retval * i; 
end; 

fonction factorial_with_while(n : integer): integer; 
var i :  integer; 
begin
   retval := 1; 
   i := 1; 
   repeat 
      //writeln('factorial_with_while - i: ', i); 
      retval := retval * i;
      i := i + 1; 
   until i > n; 
end; 


function sum_of_int__rec(n : integer):integer; 
begin
   if n <= 0 then retval := 0; 
   //else retval := n + sum_of_int__rec(n - 1);  //rec(n - 1);  
   else retval := n + rec(n - 1);  //rec(n - 1);  
end; 

function sum_of_int__for(n : integer):integer; 
var i :  integer; 
begin
   retval := 0; 
   for i := 1 to n do 
      retval := retval + i; 
end; 

function sum_of_int__while(n : integer):integer; 
var i :  integer; 
begin
   retval := 0; 
   i := 0; 
   repeat 
      retval := retval + i; 
      i := i + 1; 
   until i > n; 
end; 



begin 
   Writeln('Factorial(10)            = ', factorial(10)); 
   Writeln('Factorial_with_for(10)   = ', factorial_with_for(10)); 
   Writeln('Factorial_with_while(10) = ', factorial_with_while(10)); 

   Writeln('Sum_of_int__rec(10)   = ', sum_of_int__rec(10)); 
   Writeln('Sum_of_int__for(10)   = ', sum_of_int__for(10)); 
   Writeln('Sum_of_int__while(10) = ', sum_of_int__while(10)); 
end; 

// ------------------------------------------------------------------- 


procedure add1(var n : integer);
begin 
   n := n + 1;
end; 

procedure swap1(var a : integer; var b: integer); 
  var c :  integer; 
begin 
   c := a; 
   a := b; 
   b := c; 
end; 

procedure swap_string(var a : string; var b: string); 
  var c :  string; 
begin 
   c := a; 
   a := b; 
   b := c; 
end; 

var u : integer; 
begin
   u := 10; 
   Add1(u); 
   Writeln('Add1(10) = ', u); 
end; 

var v,v1: integer; 
begin 
   v := 42; 
   Writeln('u,v = ', u, ', ', v); 
   swap1(u,v); 
   Writeln('Swap!'); 
   Writeln('u,v = ', u, ', ', v); 
end; 


// ------------------------------------------------------------------- 

type TPoint3D =  record x : float; y: float; z: float; end; 

 var w1, w2 : TPoint3D; 
begin
   Writeln('W1.x = ', w1.x); 
   W1.x := 3.0; 
   Writeln('W1.x = ', w1.x); 
   W1.y := -5; 
   Writeln('W1.y = ', w1.y); 
   W1.y := -5.5 + 0.0 + w1.x - w1.y; 
   Writeln('W1.y = ', w1.y); 
end;

function TPoint3D_make(x : float; y: float; z: float): TPoint3D; 
begin 
   WRITELN('BEGIN OF TPoint3D_make'); 
   retval.x := x; 
   retval.y := y; 
   retval.z := z; 
   WRITELN('END OF TPoint3D_make'); 
   Writeln('retval = ', retval); 
end; 

//function TPoint3D_make(x : float; y: float; z: float): TPoint3D; begin    retval.x := x;    retval.y := y;    retval.z := z;    end; 


begin
   w2 := TPoint3D_make(1, 2, 3); 
   Writeln('W2.x = ', w2.x); 
   Writeln('W2.y = ', w2.y); 
   Writeln('W2.z = ', w2.z); 
   Writeln('W2 = ', w2); 

   Writeln('W2 = ', 2 * w2); 
   Writeln('W2 = ', .5 * w2); 
   Writeln('W2 = ', w2 * -4); 
   Writeln('W2 = ', w2 * -0.2); 

   Writeln('W2 = ', 2 + w2); 
   Writeln('W2 = ', .5 + w2); 
   Writeln('W2 = ', w2 + -4); 
   Writeln('W2 = ', w2 + -0.2); 

   Writeln('W2 = ', - w2); 
   Writeln('W2 = ', 0 - w2); 
   Writeln('W2 = ', 2 - w2); 
   Writeln('W2 = ', .5 - w2); 
   Writeln('W2 = ', w2 - 4); 
   Writeln('W2 = ', w2 - -4); 
   Writeln('W2 = ', w2 - -0.2); 

   Writeln('W2 = ', w2 + w2); 
   Writeln('W2 = ', w2 - w2); 
   Writeln('W2 = ', - w2 + w2); 
   Writeln('W2 = ', w2 * w2); 
end;




// ------------------------------------------------------------------- 
methode Trace of TPoint3D(): float; 
begin 
   (retval) := this.x + this.y + this.z; 
end; 

//methode Trace of TPoint3D(): float; begin    (retval) := this.x + this.y + this.z; end; 

methode Norme2_2 of TPoint3D(): float; 
begin
   //retval := 0; 
   //(retval) := 0; 
   //retval := this -> Trace(); 
   //retval := (this) -> Trace(); 
   // RL: TODO XXX FIXME: Object should be allocated in the memory_frame and not on the stack. 
   // RL: Problem: The resulting type of 'this * this' is not computed at compile-time. 
   //     Therefore, at compile-time, we do not know the size of the resulting value. 
   //     ... 
   //     Well, nothing is great here. 
   //     If the type was compile-time computed, then 'pscript_value' would not have to allocate any memory. 
   //     If the type is only known when 'pscript_value' happens, then it could make sense that 'pscript_value' allocates the needed memory, even though I do not like that much. 
   //     ... 
   //     Whyt not. 
   //     ... 
   //     In the meantime, we'll do a hack: when the method will be called, if the object is on the stack, it moves it in the local memory frame (a cast issue...! casting an object to a proxy to that objet, casting issueswhich requires to the ability to allocate memory) 
   //     ... 
   //     Nothing is great. Ugly hack in the meantime.  
   //     ... 
   retval := (this * this) -> Trace(); 
end; 



var t:float; 
begin 
   t := W2 -> Trace(); 
   Writeln('W2 -> Trace = ', t); 
end; 

begin 
   Writeln('W2 -> Trace = ', W2 -> Trace()); 
end; 

begin  
   Writeln('W2 -> Norme2_2 = ', W2 -> Norme2_2()); 
end; 


methode Scale of TPoint3D(factor : float); 
begin
   //writeln('TPoint3D::SCALE - THIS = ', this); 
   this := factor * this; 
   //writeln('TPoint3D::SCALE - THIS = ', this); 
end; 

begin  
   Writeln('W2 = ', w2); 
   w2 -> Scale(2); 
   Writeln('W2 = ', w2); 
end; 

begin    
   W2 -> printz(); 
end;


begin 
   Writeln('W2 = ', w2); 
end; 


// ------------------------------------------------------------------- 
const N = 2; 
const M = 2 + 4; 
const Q = 2 + N * 3 - M; 
const P	= "Qu'il prénomma Gustave, Alphonse, Arthur et Philibert."; 
const R	= 2.5; 
const S	= nil; 
 
begin 
   Writeln('Const N = ', N); 
   Writeln('Const M = ', M); 
   Writeln('Const Q = ', Q); 
   Writeln('Const P = ', P); 
   Writeln('Const R = ', R); 
   Writeln('Const S = ', S); 
end;


// ------------------------------------------------------------------- 

procedure writeln_wrapup_for_map(s : string);
begin 
   //Writeln('writeln_wrapup_for_map CALLED!'); 
   writeln(s); 
end; 

begin
   Writeln('W2 = ', w2); 
   Writeln('FACTORIAL(W2) = ', map(factorial,w2)); 
   
   Writeln('MAP = ', map(factorial,TPoint3D_make(9,10,0))); 

   writeln_wrapup_for_map(3.0); 
   map(writeln_wrapup_for_map,w2); 

   Writeln('MAP = ', map(factorial,TPoint3D_make(9,10,0))); 
 
end;

 
// ------------------------------------------------------------------- 


var b:2..8; 
var wb:[2..8*8]; 

var c:8..2; 

begin 
   b :=     4; writeln('b = ', b); 
   b :=    -9; writeln('b = ', b); 
   b :=    42; writeln('b = ', b); 
   b := 3-1+4; writeln('b = ', b); 
end;

var bb:[-1..1];
var cc:[1..-1];
var bbb	: [-15..-7];

begin 
   bb  :=  -2; writeln('bb  = ', bb ); 
   bb  :=   0; writeln('bb  = ', bb ); 
   bbb := -10; writeln('bbb = ', bbb); 
end; 


// ------------------------------------------------------------------- 
begin 
   writeln('Newline = "', newline, '"'); 
end; 

// ------------------------------------------------------------------- 

var a : array[2..4] of string; 

begin 
   a[2] := 'Hello'; 
   writeln('a[2] = ', a[2]); 
   a[3] := 'World'; 
   writeln('a[2], a[3] = ', a[2], ' ', a[3]); 
   swap_string(a[2], a[3]); 
   writeln('a[2], a[3] = ', a[2], ' ', a[3]); 
   writeln('a = ', a); 
/*
   */
end; 

 

// ------------------------------------------------------------------- 
var d : array[10] of integer; 

procedure print_array(u	: array[10] of integer)
var i : integer; 
begin
   for i := 0 to 9 do 
      writeln('u[', i, '] = ', u[i]); 
end;

procedure init_array(var u	: array[10] of integer)
var i : integer; 
begin
   for i := 0 to 9 do 
      u[i] := i; 
end;

begin 
   d[0] := 0; 
   d[1] := 1; 
   d[2] := 2; 
   d[3] := 3; 
   d[4] := 4; 
   d[5] := 5; 
   d[6] := 6; 
   d[7] := 7; 
   d[8] := 8; 
   d[9] := 9; 
   writeln(d); 
   print_array(d); 
   d := d - d; 
   writeln(d); 
   init_array(d); 
   writeln(d); 
   map(writeln_wrapup_for_map,d); 
   writeln(d); 
   writeln(map(factorial,d)); 
   writeln(d); 
   d := map(factorial,d); 
   writeln(d); 
end; 

begin
   d[-1] := 0; 
end; 

begin
   d[10] := 0; 
end; 

 

// ------------------------------------------------------------------- 
  var REPONSES0 : array[-1..1] of string; 

  const Perso = "Albert"; 

procedure Puits_boss_parler(); 
  var REPONSES : array[-1..1] of string; 
begin 
   REPONSES[-1]:='ERREUR'; 
   REPONSES[0]:="As-tu seulement un seau pour la porter cette eau ? Gros malin."; 
   REPONSES[1]:="Reviens me voir quand tu veux."; 
   writeln("Boujour ",Perso, "! C'est moi qui m'occupe du puits."); 
/*
   writeln(REPONSES[Menu("Tu veux de l'eau ?") 
	   begin 
	     item('Oui'); 
	     item('Non'); 
	   end]);
*/
end; 
 

begin
   //Puits_boss_parler(); 
end; 
 

// ------------------------------------------------------------------- 




//var u: integer; 
procedure f()
begin
   u := u + 1; 
end; 

procedure g() ;
var u: integer; 
begin 
   f(); 
end; 

begin 
   u := 0; 
   g(); 
   writeln('u = ', u); 
end;



// ------------------------------------------------------------------- 

//begin v := (lambda (): integer; begin retval := 1; end)(); end; 


begin 
   writeln('v = ', v); 
   writeln('Defining lambda-object BEGIN'); 
   v := (lambda (): integer; begin retval := 1; end)(); 
   writeln('Defining lambda-object END'); 
   writeln('v = ', v); 
end; 


var uu: variant; 
begin 
   writeln('VARIANT BEGIN'); 
   writeln('uu = ', uu); 
   uu := 1; 
   writeln('uu = ', uu); 
   variant_dispose(uu); 
   writeln('uu = ', uu); 
   uu := 3.14; 
   writeln('uu = ', uu); 
   //uu := uu + 10; 
   writeln('uu = ', uu); 
   writeln('VARIANT END'); 
end; 

begin 
   writeln('typeof(u) = ', typeof(u)); 
   writeln('typeof(uu) = ', typeof(uu)); 
   writeln('typeof(w2) = ', typeof(w2)); 
end; 

begin 
   writeln('sizeof(u) = ', sizeof(u)); 
   writeln('sizeof(uu) = ', sizeof(uu)); 
   writeln('sizeof(w2) = ', sizeof(w2)); 
end; 

begin 
   writeln('typeof(factorial) = ', typeof(factorial)); 
   writeln('sizeof(factorial) = ', sizeof(factorial)); 
end; 

var vv: variant; 
begin 
   writeln('Defining lambda-object BEGIN'); 
   writeln('vv = ', vv); 
   vv := lambda (): integer; begin retval := 1; end; 
   writeln('vv = ', vv); 
   writeln('vv() = ', vv()); 
   writeln('Defining lambda-object END'); 
end; 

begin 
   writeln('typeof(<lambda>) = ', typeof(lambda (): integer; begin retval := 1; end)); 
   writeln('sizeof(<lambda>) = ', sizeof(lambda (): integer; begin retval := 1; end)); 
end; 


// ------------------------------------------------------------------- 

begin
   writeln('nil = ', nil); 
end; 

begin
   writeln('NULL = ', NULL); 
end; 


// ------------------------------------------------------------------- 

var ww; 
begin
   ww := cons(1, nil); 
   writeln("ww = ", ww); 
end; 

begin
   writeln("... = ", cons(1, cons("HW", cons(3.14, nil)))); 
end; 

begin
   writeln("... = ", cons(1, cons(3.14, nil))); 
end; 

begin
   writeln("... = ", cons("HW", nil)); 
end; 
begin
   writeln("CONS = ", cons(lambda (): integer; begin retval := 1; end, nil)); 
end; 

begin
   writeln("... = ", cons(1, cons("HW", nil))); 
end; 

begin
   writeln("... = ", cons("HW", cons(3.14, nil))); 
end; 

begin
   writeln("... = ", cons(cons("HW", cons(3.14, nil)),nil)); 
end; 

begin
   writeln("... = ", car(cons("HW", nil))); 
end; 
begin
   writeln("... = ", car(cons(nil, nil))); 
end; 
begin
   writeln("... = ", car(cons(cons(2,nil), nil))); 
end; 

begin
   writeln("... = ", cdr(cons("HW", nil))); 
end; 
begin
   writeln("... = ", cdr(cons(nil, nil))); 
end; 
begin
   writeln("... = ", cdr(cons(cons(2,nil), nil))); 
end; 

begin
   writeln("... = ", cdr(cons(nil, "HW"))); 
end; 
begin
   writeln("... = ", cdr(cons(nil, nil))); 
end; 
begin
   writeln("... = ", cdr(cons(nil, cons(2,nil)))); 
end; 

begin
   writeln("LIST = ", list(1, 2, 3, 4, 5, 6, 7, 8)); 
end; 
begin
   writeln("LIST = ", list()); 
end; 
begin
   writeln("LIST = ", list(1, "\"2", 3.0, '''4', false, 6, nil, 7, NULL, 8, true, 9, lambda (): integer; begin retval := 1; end, 10)); // \"
end; 

begin
   writeln("ARRAY = ", array_make(1, 2, 3, 4, 5, 6, 7, 8)); 
end; 
begin
   writeln("ARRAY = ", array_make()); 
end; 
begin
   writeln("ARRAY = ", array_make(1, "\"2", 3.0, '''4', false, 6, nil, 7, NULL, 8, true, 9, lambda (): integer; begin retval := 1; end, 10));  // \"
end; 





// ------------------------------------------------------------------- 














// ------------------------------------------------------------------- 

begin
   writeln('End Of File'); 
end; 




