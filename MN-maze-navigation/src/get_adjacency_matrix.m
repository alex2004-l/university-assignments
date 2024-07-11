function [Adj] = get_adjacency_matrix (Labyrinth)
	% Labyrinth -> the matrix of all encodings
  
  % Adj -> the adjacency matrix associated to the given labyrinth
  
  [m n] = size(Labyrinth);
  
  win = m*n + 1;
  lose = m*n + 2;
  true = dec2bin(0);
  k = 1;
  
  % binary_enc este o matrice cu toate codificarile binare ale
  % matricei Labyrinth primita ca argument
  binary_enc = dec2bin(Labyrinth.');
  
  for i = 1: m*n
    
    % se verifica primul element de pe rand - corespunde directiei Nord
    if binary_enc(i,1) == true
      row(k) = i;
      % daca i este un element de pe primul rand al matricei initiale
      % inseamna ca exista o iesire castigatoare din labirint in directia N
      i <= m && (column(k++) = win) || (column(k++) = i-m);
    endif
    
    % se verifica al doilea element de pe rand - corespunde directiei Sud
    if binary_enc(i,2) == true
      row(k) = i;
      % daca i este un element de pe ultimul rand al matricei initiale
      % inseamna ca exista o iesire castigatoare din labirint in directia S
      i > m * (n-1) && (column(k++) = win) || (column(k++) = i+m);
    endif
    
    % se verifica al treilea element de pe rand - corespunde directiei Est
    if binary_enc(i,3) == true
      row(k) = i;
      % daca i este un element de pe ultima coloana a matricei initiale
      % inseamna ca exista o iesire pierzatoare din labirint in directia E
      mod(i,m) == 0 && (column(k++) = lose) || (column(k++) = i+1);
    endif
    
    % se verifica al patrulea element de pe rand - corespunde directiei Vest
    if binary_enc(i,4) == true
      row(k) = i;
      % daca i este un element de pe prima coloana a matricei initiale
      % inseamna ca exista o iesire pierzatoare din labirint in directia V
      mod(i,m) == 1 && (column(k++) = lose) || (column(k++) = i-1);
    endif
    
  endfor
  
  row(k) = column(k) = win;
  row(k+1) = column(k+1) = lose;
  value(1:k+1) = 1;
  
  % se creaza matricea rara Adj
  Adj = sparse(row,column,value);
  
endfunction
