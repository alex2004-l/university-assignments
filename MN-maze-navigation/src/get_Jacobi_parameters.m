function [G, c] = get_Jacobi_parameters (Link)
	% Link -> the link matrix (edge with probabilities matrix)
	
  % G -> iteration matrix
	% c -> iteration vector
	
  [m m] = size(Link);
  
  lose = m;
  win = m -1;
  
  % se elimina din G coloane corespunzatoare win si lose
  G = Link.';
  G(lose,:) = [];
  G(win, :) = [];
  
  % se elimina din G randurile corespunzatoare win si lose
  G = G.';
  G(lose,:) = [];
  G(win, :) = [];
  
  c = Link(1:win-1, win);
endfunction
