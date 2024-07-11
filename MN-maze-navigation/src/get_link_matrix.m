function [Link] = get_link_matrix (Labyrinth)
	% Labyrinth -> the matrix of all encodings
  
  % Link -> the link matrix associated to the given labyrinth
  
  [m n] = size(Labyrinth);
  
  win = m * n + 1;
  lose = m * n + 2;
  true = dec2bin(0);
  k = 1;
  
  binary_enc = dec2bin(Labyrinth.');
  
  for i = 1: m * n
    % count reprezinta numarul de directii posibile in care poate merge
    % robotul odata ajuns pe casuta i
    count = sum(binary_enc(i,1:4) == true);
    
    if binary_enc(i,1) == true
      row(k) = i;
      i <= m && (column(k++) = win) || (column(k++) = i-m);
    endif
    
    if binary_enc(i,2) == true
      row(k) = i;
      i > m*(n-1) && (column(k++) = win) || (column(k++) = i+m);
    endif
    
    if binary_enc(i,3) == true
      row(k) = i;
      mod(i,m) == 0 && (column(k++) = lose) || (column(k++) = i+1);
    endif
    
    if binary_enc(i,4) == true
      row(k) = i;
      mod(i,m) == 1 && (column(k++) = lose) || (column(k++) = i-1);
    endif
    
    % vectorul value nu mai contine doar valori de 1, ca la matricea de adiacenta
    % pentru randul i - valorile sunt 1/nr_pozitie_urm_posibila
    value(k-count:k-1) = 1/count;
  endfor
  
  row(k) = column(k) = win;
  row(k+1) = column(k+1) = lose;
  value(k:k+1) = 1;
  
  % se creeaza matricea rara din cei trei vectori
  Link = sparse(row, column, value);
  
endfunction
