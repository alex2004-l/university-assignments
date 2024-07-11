case class Database(tables: List[Table]) {
  override def toString: String = tables.foldRight("")(_.toString + "\n" + _)

  def create(tableName: String): Database = {
    // verifica daca exista o tabela cu numele dat, si in caz negativ creeaza o tabela goala
    val exists = tables.foldRight(false){(table, acc) =>
      if (table.tableName.equals(tableName)) true else acc}
    if(exists) this
    else Database(tables :+ Table(tableName, ""))
  }

  def drop(tableName: String): Database = {
    // elimina din lista tabelul cu numele care coincide argumentul primit
    val newTables = tables.foldRight(Nil:List[Table])((table, acc) =>
      if (table.tableName.equals(tableName)) acc else table :: acc)
    Database(newTables)
  }

  def selectTables(tableNames: List[String]): Option[Database] = {
    // filtreaza tabelele, verificand daca numele acestora se afla in lista primita
    val result = tables.filter(tab => tableNames.contains(tab.tableName))

    if (result.isEmpty) None
    else Some(Database(result))
  }

  def join(table1: String, c1: String, table2: String, c2: String): Option[Table] = {
    val tab1 = selectTables(List(table1))
    val tab2 = selectTables(List(table2))
    (tab1, tab2) match {
      /* cel putin una din tabele nu exista in baza de date*/
      case (None, _) => None
      case (_, None) => None
      /* ambele tabele exista */
      case (Some(a), Some(b)) =>
        /* cel putin una dintre tabele este goala */
        val t1 = a(0)
        val t2 = b(0)
        if (t1.tableData.isEmpty)
          Some(t2)
        else if (t2.tableData.isEmpty)
          Some(t1)
        else {
          val accumulatorNewData = (Nil: List[Row], Nil: List[Row], Nil: List[Row])
          /* Lista cu map-urile concatenate atunci cand c1 se gaseste si in tabelul 2 */
          val newData = t1.tableData.foldRight(accumulatorNewData) { (map, acc) =>
            /* first si second -> liste cu map-urile deja incluse */
            val (result, first, second) = acc
            val stringToLook = map.getOrElse(c1, "")
            val filterName = Field(c2, _.equals(stringToLook))
            val SecondTableFiltered = t2.filter(filterName)
            /* daca valoarea cheii din primul tabel de afla si in al doilea */
            if (SecondTableFiltered.data.nonEmpty) {
              /* se elimina coloana comuna din al doilea tabel si se imbina map-urile */
              val filteredWithoutField = SecondTableFiltered.data.head.filter { case (key, _) => !key.equals(c2) }
              val newMap = map ++ filteredWithoutField.map {
                case (key, value) => key -> (map.get(key) match {
                  case Some(v) => if (!v.equals(value)) v + ";" + value else value
                  case None => value
                })
              }
              (newMap :: result, map :: first, SecondTableFiltered.data.head :: second)
            } else acc
          }
          /* se pastreaza din tabele randurile care inca nu au fost incluse */
          val notList1 = t1.tableData.filterNot(map1 => newData._2.contains(map1))
          val notList2 = t2.tableData.filterNot(map2 => newData._3.contains(map2))

          /* se adauga campurile lipsa pentru randurile care inca nu au fost adaugate */
          val newDataHeader = t1.header ++ t2.header.filterNot(_.equals(c2))
          val h1 = notList1.map(m => newDataHeader.foldLeft(Map.empty[String, String])
          ((acc, str) => acc + (str -> m.getOrElse(str, ""))))
          val h2 = notList2.map(m => newDataHeader.foldLeft(Map.empty[String, String]) { (acc, str) =>
            if (str.equals(c1)) acc + (c1 -> m.getOrElse(c2, ""))
            else acc + (str -> m.getOrElse(str, ""))
          })
          /* se concateneaza cele trei liste */
          Some(Table(t1.tableName, newData._1 ++ h1 ++ h2))
        }
    }
  }

  // functia de indexare
  def apply(index: Int): Table = {
    tables.apply(index)
  }
}
