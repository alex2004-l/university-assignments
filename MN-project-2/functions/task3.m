## Copyright (C) 2021 Andrei
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <https://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {} {@var{retval} =} pca_cov (@var{input1}, @var{input2})
##
## @seealso{}
## @end deftypefn

## Author: Andrei <Andrei@DESKTOP-PK505U9>
## Created: 2021-09-06

function new_X = task3 (photo, pcs)
  [m, n] = size (photo);

  % initializare matrice finala.
  new_X = zeros (m, n);

  % cast photo la double.
  photo = double(photo);

  % calculeaza media fiecarui rand al matricii.
  miu = zeros(m,1);
  for i = 1:m
    miu(i) = sum(photo(i,:))/n;
  endfor

  % normalizeaza matricea initiala scazand din ea media fiecarui rand.
  photo = photo - miu;

  % calculeaza matricea de covarianta.
  Z = 1/(n-1) * photo * photo.';

  % calculeaza vectorii si valorile proprii ale matricei de covarianta.
  [V,S] = eig(Z);

  % ordoneaza descrescator valorile proprii si creaza o matrice V
  % formata din vectorii proprii asezati pe coloane, astfel incat prima coloana
  % sa fie vectorul propriu corespunzator celei mai mari valori proprii si
  % asa mai departe.
  eigen_vals = diag(S);

  [~, SortIdx] = sort(eigen_vals, 'descend');
  V = V(:, SortIdx);

  % pastreaza doar primele pcs coloane
  % OBS: primele coloane din V reprezinta componentele principale si
  % pastrandu-le doar pe cele mai importante obtinem astfel o compresie buna
  % a datelor. Cu cat crestem numarul de componente principale claritatea
  % imaginii creste, dar de la un numar incolo diferenta nu poate fi sesizata
  % de ochiul uman asa ca pot fi eliminate.
  W = V(:, 1:pcs);

  % creaza matricea Y schimband baza matricii initiale.
  Y = W.' * photo;

  % calculeaza matricea new_X care este o aproximatie a matricii initiale
  % aduna media randurilor scazuta anterior.
  new_X = W * Y + miu;

  % transforma matricea in uint8 pentru a fi o imagine valida.
  new_X = uint8(new_X);

endfunction
