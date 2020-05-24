

begin 
  menu "Hello!" 
  begin 
     item ('How are you?'); 
     item ("How are you doing?"); 
  end; 
end; 






  var REPONSES0 : array[-1..1] of string; 

  const Perso = "Albert"; 

procedure Puits_boss_parler(); 
  var REPONSES : array[-1..1] of string; 
begin 
   REPONSES[-1]:='ERREUR'; 
   REPONSES[0]:="As-tu seulement un seau pour la porter cette eau ? Gros malin."; 
   REPONSES[1]:="Reviens me voir quand tu veux."; 
   writeln("Boujour ",Perso, "! C'est moi qui m'occupe du puits."); 
   writeln(REPONSES[Menu("Tu veux de l'eau ?") 
	   begin 
	     item('Oui'); 
	     item('Non'); 
	   end]);
end; 
 

begin
   Puits_boss_parler(); 
end; 

begin 
  Writeln('Hello World!','Hello World!','Hello World!'); 
end; 

begin
  Writeln('Raw number = ', 10); 
end; 

function factorial_fake(n	: integer):integer
begin 
   retval := 42; 
end; 

begin
  Writeln('factorial_fake(10) = ', factorial_fake(10)); 
end; 
