object Queries {

  def killJackSparrow(t: Table): Option[Table] = {
    // se filtreaza dupa coloana nume; se verifica ca numele sa nu fie "Jack"
    queryT(PP_SQL_Table_Filter(Some(t), "FILTER", Field("name", !_.equals("Jack"))))
  }

  def insertLinesThenSort(db: Database): Option[Table] = {
    // creeaza si selecteaza un nou tabel, insereaza o lista de randuri in acesta, dupa care o sorteaza
    queryT(PP_SQL_Table_Sort(queryT(PP_SQL_Table_Insert(Some(queryDB(PP_SQL_DB_Select(
      queryDB(PP_SQL_DB_Create_Drop(Some(db), "CREATE", "Inserted Fellas"))
            , "SELECT", List("Inserted Fellas"))).get(0)), "INSERT", List(
      Map("name" -> "Ana", "age" -> "93", "CNP" -> "455550555"),
      Map("name" -> "Diana", "age" -> "33", "CNP" -> "255532142"),
      Map("name" -> "Tatiana", "age" -> "55", "CNP" -> "655532132"),
      Map("name" -> "Rosmaria", "age" -> "12", "CNP" -> "855532172")
    ))), "SORT", "age"))
  }

  def youngAdultHobbiesJ(db: Database): Option[Table] = {
    // Combina cele doua tabele, filtreaza tabela rezultat dupa cele trei criterii si extrage
    // coloanele "name" si "hobby"
    queryT(PP_SQL_Table_Select(queryT(PP_SQL_Table_Filter(
      queryT(PP_SQL_Table_Filter(
      queryT(PP_SQL_Table_Filter(Some(queryDB(PP_SQL_DB_Join(
        Some(db), "JOIN", "People", "name", "Hobbies", "name")).get(0)),
        "FILTER", Field("age", str => !str.equals("") && str.toInt < 25))),
        "FILTER", Field("name", _.startsWith("J")))),
        "FILTER", Field("hobby", !_.equals("")))),
      "EXTRACT", List("name", "hobby")))
  }
}
