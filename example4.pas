begin 
a := - 1;
//Writeln('W2 = ', - w2); 
end; 

const a = 1; b = 2; a = 3; var a : integer; begin end; begin a := 2; end; procedure c() begin end; function c() begin end method string of integer () begin end;  var u : variant; begin u += 2; end 


begin
   writeln('Hello World!'); 
end; 

begin
   writeln('Hello World!'); 
end; 

procedure print_pscript()
begin 
   writeln('PSCRIPT!'); 
end; 

begin
   writeln('Hello World!'); 
end; 

begin
   print_pscript(); 
end; 
 

function fact_fake(n	: integer):integer
begin 
   retval := 42; 
end; 

function fact_fake_bis(n	: integer;;;;;):integer;;;;;;;;
begin 
   retval := 42; 
end; 
begin
   writeln('FACT_FAKE defined'); 
end; 

function fact(n	: integer):integer;
begin
   //writeln('FACT CALLED - n = ', n); 
   if n <= 0 then retval := 1; 
   else retval := n * rec(n - 1); 
end; 

begin
   writeln('FACT_REC defined'); 
end; 

begin
  Writeln('Raw number = ', 10); 
end; 

begin
  Writeln('Fact_fake(10) = ', fact_fake(10)); 
  Writeln('Fact_fake(10) = ', fact_fake(10)); 
end; 
begin
  Writeln('Fact_fake(10) = ', fact_fake(10)); 
  Writeln('Fact_fake(10) = ', fact_fake(10)); 
end; 

begin
  Writeln('Fact(3) = ', fact(3)); 
  Writeln('Fact(10) = ', fact(10)); 
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




function fact2(n	: integer):integer;
begin
   if n <= 0 then retval := 1; 
   else retval := n * fact(n - 1); 
end; 

function fact3(n	: integer):integer;
begin
   if n <= 0 then retval := 1; 
   else retval := n * fact(n - 1);  
end; 

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

fonction fact_with_for(n : integer): integer; 
var i :  integer; 
begin
   retval := 1; 
   for i := 2 to n do 
      retval := retval * i; 
end; 

fonction fact_with_while(n : integer): integer; 
var i :  integer; 
begin
   retval := 1; 
   i := 1; 
   repeat 
      //writeln('fact_with_while - i: ', i); 
      retval := retval * i;
      i := i + 1; 
   until i > n; 
end; 




function sum_of_int__rec(n : integer):integer; 
begin
   if n <= 0 then retval := 0; 
   else retval := n + rec(n - 1);  
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
   Writeln('Fact(10)            = ', fact(10)); 
   Writeln('Fact_with_for(10)   = ', fact_with_for(10)); 
   Writeln('Fact_with_while(10) = ', fact_with_while(10)); 

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

function TPoint3D_make(x : float; y: float; z: float): TPoint3D; 
begin 
   retval.x := x; 
   retval.y := y; 
   retval.z := z; 
   //WRITELN('END OF TPoint3D_make'); 
end; 

function TPoint3D_make(x : float; y: float; z: float): TPoint3D; begin    retval.x := x;    retval.y := y;    retval.z := z;    end; 


methode Trace of TPoint3D(): float; 
begin 
   (retval) := this.x + this.y + this.z; 
end; 


methode Trace of TPoint3D(): float; begin    (retval) := this.x + this.y + this.z; end; 


methode Norme2_2 of TPoint3D(): float; 
begin
   //retval := 0; 
   //(retval) := 0; 
   //retval := this -> Trace(); 
   //retval := (this) -> Trace(); 
   retval := (this * this) -> Trace(); 
end; 

methode Scale of TPoint3D(factor : float); 
begin
   //writeln('TPoint3D::SCALE - THIS = ', this); 
   this := factor * this; 
   //writeln('TPoint3D::SCALE - THIS = ', this); 
end; 

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

begin
   w2 := TPoint3D_make(1, 2, 3); 
   Writeln('W2.x = ', w2.x); 
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
   Writeln('FACTORIAL(W2) = ', map(fact,w2)); 
   
   Writeln('MAP = ', map(fact,TPoint3D_make(9,10,0))); 

   writeln_wrapup_for_map(3.0); 
   map(writeln_wrapup_for_map,w2); 

   Writeln('MAP = ', map(fact,TPoint3D_make(9,10,0))); 
 
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
   writeln(map(fact,d)); 
   writeln(d); 
   d := map(fact,d); 
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

begin 
   writeln('Defining lambda-object BEGIN'); 
   u := lambda (): integer; begin retval := 1; end; 
   writeln('Defining lambda-object END'); 
end; 

begin v := (lambda (): integer; begin retval := 1; end)(); end; 


begin 
   writeln('Defining lambda-object BEGIN'); 
   v := (lambda (): integer; begin retval := 1; end)(); 
   writeln('Defining lambda-object END'); 
end; 

// ------------------------------------------------------------------- 

// ------------------------------------------------------------------- 














// ------------------------------------------------------------------- 

begin
   writeln('End Of File'); 
end; 


