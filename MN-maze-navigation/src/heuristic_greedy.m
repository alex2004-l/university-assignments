function [path] = heuristic_greedy (start_position, probabilities, Adj)
	% start_position -> the starting point in the labyrinth
	% probabilities -> vector of associated probabilities: including WIN and LOSE
	% Adj -> adjacency matrix
	
	% path -> the states chosen by the algorithm
  
  [nr_positions a] = size(probabilities);
  % se adauga in path pozitia de start
  path(1) = start_position;
  k = 1;
  
  % vectorul care reflacta daca o pozitie a fost vizitata sau nu
  visited = zeros(nr_positions,1);
  visited(start_position) = 1;
  
  while k > 0
    position = path(k);
    
    % se verifica daca pozitia curenta este WIN
    if probabilities(position) == 1
      break;
    endif
    
    flag = 0; % indica daca mai exista un drum posibil nevizitat
    number = -1; % indicele pozitiei nevizitate cu cea mai mare probabilitate
    prob_n = 0; 
    
    for i = 1: nr_positions
      if Adj(position, i) > 0
        if visited(i) == 0
          flag = 1; % daca exista cel putin o pozitie nevizitata
          % se compara probabilitatea pozitiei cu cea retinuta in prob_n
          if probabilities(i) > prob_n
            number = i;
            prob_n = probabilities(i);
          endif
          
        endif
      endif
    endfor
    % daca nu mai exista un drum posibil nevizitat din pozitia curenta
    % pozitia este scoasa dni path
    if flag == 0
      path(k) = [];
      k--;
    else
      % altfel, este adaugata o noua pozitie in path
      visited(number) = 1;
      k ++;
      path(k) = number;
    endif
  endwhile
  path = path.';

endfunction
