type Row = Map[String, String]
type Tabular = List[Row]

case class Table (tableName: String, tableData: Tabular) {

  override def toString: String = {
    /* Transforma header-ul intr-un string unde keys sunt separate prin "," */
    val headerToString = header.mkString(",")
    /* Transforma fiecare rand intr-un string unde valorile sunt separate prin ","
    *  urmand ca randurile sa fie concatenate cu "\n" intre */
    val dataToString = data.map(_.values.mkString(",")).mkString("\n")

    headerToString + "\n" + dataToString
  }

  def insert(row: Row): Table = {
    /* Verifica daca rand-ul deja exista in tabel, si in caz negativ il insereaza*/
    if (tableData.contains(row)) this
    else Table(tableName, tableData :+ row)
  }

  def delete(row: Row): Table = {
    /* Elimina din tabel randurile egale cu row */
    val deleteData = tableData.filterNot(_.equals(row))
    Table(tableName, deleteData)
  }

  def sort(column: String): Table = {
    /* sorteaza randurile din tabel in functie de string-ul coloana */
    val sortedData = tableData.sortBy(_(column))
    Table(tableName, sortedData)
  }

  def update(f: FilterCond, updates: Map[String, String]): Table = {
    val updatedData = data.foldRight(Nil:List[Row]){(row, acc) =>
      /* pentru randurile se evalueaza functia din filtru */
      f.eval(row) match {
        case Some(value) => {
          /* se respecta conditia din filtru */
          if(value) {
            val updatedRow = row.map{(key,value) =>
              /* daca nu exista cheie asociata in map-ul de updates,
              * se pastreaza cheia veche */
              updates.get(key) match {
                case Some(newValue) => (key, newValue)
                case None => (key, value)
              }
            }
            updatedRow::acc
          }
          else row::acc
        }
        case None => row::acc
      }
    }
    Table(tableName, updatedData)
  }

  def filter(f: FilterCond): Table = {
    val filteredData = data.foldRight(Nil:List[Row]){(row, acc) =>
      /* daca randurile respecta conditia din filtru, sunt adaugate in acumulator */
      f.eval(row) match {
        case Some(value) => if(value) row::acc else acc
        case None => acc
      }
    }
    Table(name, filteredData)
  }

  def select(columns: List[String]): Table = {
    /* se selecteaza din fiecare rand doar perechile cheie-valoare
    * ce au drept cheie unul din string-urile primite ca parametru */

    //val selectedData = data.map(_.filterKeys(columns.toSet).toMap)
    val selectedData = data.map(_.filter{case (key, value) => columns.contains(key)})
    Table(tableName, selectedData)
  }

  /* pentru header se verifica ca lista sa nu fie vida pentru a evita erori */
  def header: List[String] = if (tableData.nonEmpty) tableData.head.keySet.toList else List()
  def data: Tabular = tableData
  def name: String = tableName
}

object Table {
  def apply(name: String, s: String): Table = {
    /* String-ul primit cu tabela se separa intr-o lista
    * avand ca elemente fiecare rand sub forma de lista */
    val data = s.split("\n").toList.map(_.split(",").toList)
    /* se separa header-ul de valori */
    val header = data.head
    val tableValues = data.tail
    /* se creeaza un map pentru fiecare rand */
    val tableData = tableValues.map(header.zip(_).toMap)

    Table(name, tableData)
  }
}

extension (table: Table) {
  def apply(i: Int): Table = {
    /* functia de indexare */
    Table(table.tableName, List(table.tableData.apply(i)))
  }
}
