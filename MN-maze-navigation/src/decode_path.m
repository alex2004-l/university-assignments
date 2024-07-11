function [decoded_path] = decode_path (path, lines, cols)
	% path -> vector containing the order of the states that arrived
	% 		 to a winning position
	% lines -> numeber of lines
	% cols -> number of columns
	
	% decoded_path -> vector of pairs (line_index, column_index)
  %                 corresponding to path states

  [n l] = size(path);
  for i = 1:n-1
    % caz 1 - indicele i corespunde unui element de pe ultima coloana
    if mod(path(i),lines) == 0
      decoded_path(i,1) = path(i)/lines;
      decoded_path(i,2) = lines;
    else %caz 2 - nu se afla pe ultima coloana
      decoded_path(i,1) = floor(path(i)/lines) + 1;
      decoded_path(i,2) = mod(path(i), lines);
    endif
  endfor
endfunction