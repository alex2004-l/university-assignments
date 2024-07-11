function [x, err, steps] = perform_iterative (G, c, x0, tol, max_steps)
	% G -> iteration matrix
	% c -> iteration vector
	% x0 -> initial solution
	% tol -> accepted tolerance (norm of the difference between two solutions)
	% max_steps -> the maximum number of iterations
	
	% x -> final solution
	% err -> last calculated difference (as an euclidean norm)
	% steps -> actual number of steps performed

  [n n] = size(G);
  x = zeros(n,1);
  steps = 0;
  
  while steps < max_steps
    steps ++;
    % se gaseste urmatoarea solutie a sistemului
    x = G * x0 + c;
    err = norm(x-x0);
    % cand eroarea este mai mica decat tol, se iese din bucla while
    if err < tol
      break;
    endif
    x0 = x;
  endwhile

endfunction
