/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * SourceMod (C)2004-2014 AlliedModders LLC.  Todos los derechos reservados.
 * =============================================================================
 *
 * Este archivo es parte del SDK de SourceMod/SourcePawn.
 *
 * Este programa es software libre; puedes redistribuirlo y/o modificarlo bajo
 * los términos de la Licencia Pública General de GNU, versión 3.0, según lo
 * publicado por la Free Software Foundation.
 * 
 * Este programa se distribuye con la esperanza de que sea útil, pero SIN
 * NINGUNA GARANTÍA; ni siquiera la garantía implícita de COMERCIABILIDAD o
 * APTITUD PARA UN PROPÓSITO PARTICULAR. Consulta la Licencia Pública General de
 * GNU para obtener más detalles.
 *
 * Deberías haber recibido una copia de la Licencia Pública General de GNU junto
 * con este programa. Si no, consulta <http://www.gnu.org/licenses/>.
 *
 * Como una excepción especial, AlliedModders LLC te da permiso para enlazar el
 * código de este programa (así como sus trabajos derivados) a "Half-Life 2," el
 * "Source Engine," el "SourcePawn JIT," y cualquier MOD de juego que funcione en
 * software de Valve Corporation. Debes obedecer la Licencia Pública General de
 * GNU en todos los aspectos para el resto del código utilizado. Además, AlliedModders
 * LLC otorga esta excepción a todos los trabajos derivados. AlliedModders LLC define
 * excepciones adicionales, encontradas en LICENSE.txt (hasta la fecha, versión
 * JULY-31-2007), o en <http://www.sourcemod.net/license.php>.
 *
 * Versión: $Id$
 */

#if defined _dbi_included
 #endinput
#endif
#define _dbi_included

/**
 * Describe el estado de obtención de un campo de la base de datos.
 */
enum DBResult
{
	DBVal_Error = 0,        /**< Número de columna/campo no válido. */
	DBVal_TypeMismatch = 1, /**< No puedes recuperar estos datos con este tipo. */
	DBVal_Null = 2,         /**< El campo no tiene datos (NULL) */
	DBVal_Data = 3          /**< El campo tiene datos */
};

/**
 * Describe tipos de enlace.
 */
enum DBBindType
{
	DBBind_Int = 0,         /**< Enlaza un entero. */
	DBBind_Float = 1,       /**< Enlaza un flotante. */
	DBBind_String = 2       /**< Enlaza una cadena. */
};

/**
 * Nivel de prioridad de subprocesos.
 */
enum DBPriority
{
	DBPrio_High = 0,        /**< Prioridad alta. */
	DBPrio_Normal = 1,      /**< Prioridad normal. */
	DBPrio_Low = 2          /**< Prioridad baja. */
};

// Un Driver representa un backend de base de datos, actualmente MySQL o SQLite.
//
// Los controladores no pueden cerrarse.
methodmap DBDriver < Handle
{
	// Encuentra el controlador asociado a un nombre.
	//
	// Cadenas de controlador compatibles:
	//    mysql
	//    sqlite
	//
	// @param name          Cadena de identificación del controlador, o una cadena
	//                      vacía para devolver el controlador predeterminado.
	// @return              Identificador del controlador, o nulo en caso de fallo.
	public static native DBDriver Find(const char[] name = "");

	// Recupera una cadena de identificación de un controlador.
	//
	// Ejemplo: "mysql", "sqlite"
	//
	// @param ident         Buffer de cadena de identificación.
	// @param maxlength     Longitud máxima del buffer.
	public native void GetIdentifier(char[] ident, int maxlength);

	// Recupera una cadena de producto de un controlador.
	//
	// Ejemplo: "MySQL", "SQLite"
	//
	// @param product       Buffer de cadena de producto.
	// @param maxlength     Longitud máxima del buffer.
	public native void GetProduct(char[] product, int maxlength);
};

// Representa un conjunto de resultados devueltos de ejecutar una consulta.
methodmap DBResultSet < Handle
{
	// Avanza al siguiente conjunto de resultados.
	//
	// En algunas implementaciones de SQL, pueden existir múltiples conjuntos
	// de resultados en una consulta. Esto es posible en MySQL con consultas
	// simples cuando se ejecuta una consulta CALL. Si este es el caso, todos
	// los conjuntos de resultados deben procesarse antes de hacer otra consulta.
	//
	// @return             Verdadero si había otro conjunto de resultados, falso en caso contrario.
	public native bool FetchMoreResults();

	// Devuelve si existe o no un conjunto de resultados. Esto devolverá
	// verdadero incluso si se devolvieron 0 resultados, pero falso
	// en consultas como UPDATE, INSERT o DELETE.
	property bool HasResults {
		public native get();
	}

	// Recupera el número de filas en el último conjunto de resultados.
	// 
	// @param query        Un identificador de consulta (o declaración).
	// @return             Número de filas en el conjunto de resultados actual.
	property int RowCount {
		public native get();
	}

	// Recupera el número de campos en el último conjunto de resultados.
	property int FieldCount {
		public native get();
	}

	// Devuelve el número de filas afectadas por la consulta que generó este
	// conjunto de resultados.
	property int AffectedRows {
		public native get();
	}

	// Devuelve el ID de inserción de la consulta que generó este conjunto de resultados.
	property int InsertId {
		public native get();
	}

	// Recupera el nombre de un campo por su índice.
	// 
	// @param field        Número del campo (comenzando desde 0).
	// @param name         Buffer del nombre.
	// @param maxlength    Longitud máxima del buffer del nombre.
	// @error              Índice de campo no válido o sin conjunto de resultados actual.
	public native void FieldNumToName(int field, char[] name, int maxlength);

	// Recupera un índice de campo por nombre.
	// 
	// @param name         Nombre del campo (sensible a mayúsculas y minúsculas).
	// @param field        Variable para almacenar el índice del campo.
	// @return             Verdadero si se encontró, falso en caso contrario.
	// @error              Sin conjunto de resultados actual.
	public native bool FieldNameToNum(const char[] name, int &field);

	// Recupera una fila del conjunto de resultados actual. Esto debe ser
	// llamado con éxito antes de obtener cualquier resultado.
	//
	// Si esta función falla, se puede usar MoreRows para
	// verificar si hubo un error o si el conjunto de resultados ha terminado.
	// 
	// @return             Verdadero si se obtuvo una fila, falso en caso contrario.
	public native bool FetchRow();

	// Indica si hay más filas.
	// 
	// @return             Verdadero si hay más filas, falso en caso contrario.
	property bool MoreRows {
		public native get();
	}

	// Regresa al primer resultado en un conjunto de resultados.
	// 
	// @return             Verdadero en éxito, falso en caso contrario.
	// @error              Sin conjunto de resultados actual.
	public native bool Rewind();

	// Recupera una cadena de un campo en la fila actual de un conjunto de resultados.  
	// Si el resultado es NULL, se devolverá una cadena vacía. Se puede realizar una 
	// verificación NULL con el parámetro de resultado o con SQL_IsFieldNull().
	// 
	// @param field        El índice del campo (comenzando desde 0).
	// @param buffer       Buffer de la cadena.
	// @param maxlength    Tamaño máximo del buffer de la cadena.
	// @param result       Variable opcional para almacenar el estado del valor devuelto.
	// @return             Número de bytes escritos.
	// @error              Índice de campo no válido, conversión de tipo inválida 
	//                     desde la base de datos, o sin conjunto de resultados actual.
	public native int FetchString(int field, char[] buffer, int maxlength, DBResult &result=DBVal_Error);

	// Recupera un flotante de un campo en la fila actual de un conjunto de resultados.
	// Si el resultado es NULL, se devolverá un valor de 0.0. Se puede hacer una
	// verificación NULL con el parámetro de resultado o con SQL_IsFieldNull().
	// 
	// @param field        El índice del campo (comenzando desde 0).
	// @param result       Variable opcional para almacenar el estado del valor devuelto.
	// @return             Un valor flotante.
	// @error              Índice de campo no válido, conversión de tipo inválida 
	//                     desde la base de datos, o sin conjunto de resultados actual.
	public native float FetchFloat(int field, DBResult &result=DBVal_Error);

	// Recupera un entero de un campo en la fila actual de un conjunto de resultados.
	// Si el resultado es NULL, se devolverá un valor de 0. Se puede hacer una
	// verificación NULL con el parámetro de resultado o con SQL_IsFieldNull().
	// 
	// @param field        El índice del campo (comenzando desde 0).
	// @param result       Variable opcional para almacenar el estado del valor devuelto.
	// @return             Un valor entero.
	// @error              Índice de campo no válido, conversión de tipo inválida 
	//                     desde la base de datos, o sin conjunto de resultados actual.
	public native int FetchInt(int field, DBResult &result=DBVal_Error);

	// Devuelve si los datos de un campo en la fila actual de un conjunto de resultados 
	// son NULL o no. NULL es un tipo SQL que significa "sin datos".
	// 
	// @param field        El índice del campo (comenzando desde 0).
	// @return             Verdadero si los datos son NULL, falso en caso contrario.
	// @error              Índice de campo no válido o sin conjunto de resultados actual.
	public native bool IsFieldNull(int field);

	// Devuelve la longitud de los datos de un campo en la fila actual de un conjunto
	// de resultados. Esto solo necesita llamarse para cadenas para determinar
	// cuántos bytes utilizar. Ten en cuenta que el valor devuelto no incluye el terminador null.
	// 
	// @param field        El índice del campo (comenzando desde 0).
	// @return             Número de bytes para el tamaño de los datos del campo.
	// @error              Índice de campo no válido o sin conjunto de resultados actual.
	public native int FetchSize(int field);
};

typeset SQLTxnSuccess
{
	// Callback para una transacción exitosa.
	// 
	// @param db            Manejador de la base de datos.
	// @param data          Valor de datos pasado a SQL_ExecuteTransaction().
	// @param numQueries    Número de consultas ejecutadas en la transacción.
	// @param results       Un array de resultados de manejadores de consultas,
	//                      uno por cada consulta de numQueries. Se cierran automáticamente.
	// @param queryData     Un array de cada valor de datos pasado a SQL_AddQuery().
	function void (Database db, any data, int numQueries, Handle[] results, any[] queryData);
	
	// Callback para una transacción exitosa.
	// 
	// @param db            Manejador de la base de datos.
	// @param data          Valor de datos pasado a SQL_ExecuteTransaction().
	// @param numQueries    Número de consultas ejecutadas en la transacción.
	// @param results       Un array de resultados de DBResultSet, uno por cada
	//                      consulta de numQueries. Se cierran automáticamente.
	// @param queryData     Un array de cada valor de datos pasado a SQL_AddQuery().
	function void (Database db, any data, int numQueries, DBResultSet[] results, any[] queryData);	
}

/**
 * Callback para una transacción fallida.
 *
 * @param db            Manejador de la base de datos.
 * @param data          Valor de datos pasado a SQL_ExecuteTransaction().
 * @param numQueries    Número de consultas ejecutadas en la transacción.
 * @param error         Mensaje de error.
 * @param failIndex     Índice de la consulta que falló, o -1 si fue otro tipo de fallo.
 * @param queryData     Un array de cada valor de datos pasado a SQL_AddQuery().
 */
typedef SQLTxnFailure = function void (Database db, any data, int numQueries, const char[] error, int failIndex, any[] queryData);

// Una Transacción es una colección de declaraciones SQL que deben ejecutarse 
// exitosamente todas o ninguna.
methodmap Transaction < Handle
{
	// Crea una nueva transacción.
	public native Transaction();

	// Añade una consulta a la transacción.
	//
	// @param query        Cadena de consulta.
	// @param data         Valor de datos extra para pasar al callback final.
	// @return             Índice de la consulta en la lista de consultas de la transacción.
	public native int AddQuery(const char[] query, any data=0);
};

// Un DBStatement es una consulta SQL precompilada que se puede ejecutar múltiples 
// veces con diferentes parámetros. Un DBStatement mantiene una referencia a la 
// Base de Datos que lo preparó.
methodmap DBStatement < Handle
{
	// Asocia un parámetro en una declaración preparada con un valor entero.
	// 
	// @param param         Índice del parámetro (comienza en 0).
	// @param number        Número para asociar.
	// @param signed        Verdadero para asociar el número como firmado, falso 
	//                      para asociarlo como sin signo.
	// @error               Índice de parámetro no válido o error SQL.
	public native void BindInt(int param, int number, bool signed=true);

	// Asocia un parámetro en una declaración preparada con un valor flotante.
	// 
	// @param param         Índice del parámetro (comienza en 0).
	// @param value         Número flotante para asociar.
	// @error               Índice de parámetro no válido o error SQL.
	public native void BindFloat(int param, float value);

	// Asocia un parámetro en una declaración preparada con un valor de cadena.
	// 
	// @param param         Índice del parámetro (comienza en 0).
	// @param value         Cadena para asociar.
	// @param copy          Si SourceMod debe copiar el valor localmente si es
	//                      necesario. Si el contenido de la cadena no cambiará antes de
	//                      llamar a SQL_Execute(), esto se puede configurar en falso para optimización.
	// @error               Índice de parámetro no válido o error SQL.
	public native void BindString(int param, const char[] value, bool copy);
};

/**
 * Callback para recibir conexiones de base de datos asíncronas.
 *
 * @param db            Manejador de la conexión de la base de datos.
 * @param error         Cadena de error si hubo un problema. El error podría estar 
 *                      vacío incluso si hay una condición de error, por lo que es importante
 *                      verificar el valor real del manejador.
 * @param data          Datos pasados en la invocación original en subproceso.
 */
typedef SQLConnectCallback = function void (Database db, const char[] error, any data);

/**
 * Callback para recibir resultados de consultas asíncronas de base de datos.
 *
 * @param db            Manejador de la conexión clonada de la base de datos.
 * @param results       Objeto de resultados, o nulo en caso de error.
 * @param error         Cadena de error si hubo un problema. El error podría estar 
 *                      vacío incluso si hay una condición de error, por lo que es importante
 *                      verificar el valor real de los resultados.
 * @param data          Datos pasados en la invocación original en subproceso.
 */
typedef SQLQueryCallback = function void (Database db, DBResultSet results, const char[] error, any data);

// Una base de datos representa una conexión activa a una base de datos, ya sea a través de una
// conexión de red, un socket de dominio Unix, o un archivo abierto.
methodmap Database < Handle
{
	// Conecta a una base de datos de manera asíncrona para no bloquear el hilo del juego.
	//
	// @param callback      Callback. Si no se encuentra el controlador, el propietario es nulo.
	// @param name          Nombre de configuración de la base de datos.
	// @param data          Valor de datos extra para pasar al callback.
	public static native void Connect(SQLConnectCallback callback, const char[] name="default", any data=0);

	// Devuelve el controlador para esta conexión de base de datos.
	property DBDriver Driver {
		public native get();
	}

	// Establece el conjunto de caracteres de la conexión.
	// Similar a SET NAMES en mysql, pero permanece después de problemas de conexión.
	// 
	// Ejemplo: "utf8", "latin1"
	//
	// @param charset       Cadena del conjunto de caracteres al que cambiar.
	// @return              Verdadero si se cambió el conjunto de caracteres, falso en caso contrario.
	public native bool SetCharset(const char[] charset);

	// Escapa una cadena de base de datos para su inserción literal. Esto no es necesario
	// para asociar cadenas en declaraciones preparadas.
	//
	// Generalmente, las cadenas de base de datos se insertan en consultas entre comillas simples (').
	// Si la entrada del usuario tiene una comilla simple, debe escaparse.
	// Esta función asegura que cualquier carácter inseguro esté adecuadamente escapado.
	//
	// NOTA: SourceMod solo garantiza cadenas escapadas correctamente cuando la consulta
	// encierra la cadena entre comillas simples. Mientras los controladores suelen permitir
	// comillas dobles ("), es posible que la cadena no esté escapada correctamente (por ejemplo, en SQLite).
	//
	// @param string        Cadena a escapear.
	// @param buffer        Buffer para almacenar la cadena escapada.
	// @param maxlength     Longitud máxima del buffer.
	// @param written       Opcionalmente devuelve el número de bytes escritos.
	// @return              Verdadero en éxito, falso si el buffer no es lo suficientemente grande.
	//                      El buffer debe ser al menos 2*strlen(string)+1.
	public native bool Escape(const char[] string, char[] buffer, int maxlength, int &written=0);

	// Formatea una cadena según las reglas de formato de SourceMod (consulta la documentación).
	// Todos los especificadores de formato se escapan (consulta SQL_EscapeString) a menos que se use el indicador '!'
	//
	// @param buffer        Buffer de la cadena de destino.
	// @param maxlength     Longitud máxima del buffer de salida.
	// @param format        Reglas de formato.
	// @param ...           Número variable de parámetros de formato.
	// @return              Número de celdas escritas.
	public native int Format(char[] buffer, int maxlength, const char[] format, any ...);

	// Devuelve si una base de datos es la misma conexión que otra base de datos.
	public native bool IsSameConnection(Database other);

	// Ejecuta una consulta a través de un hilo. El manejador de resultados se pasa al callback.
	//
	// El manejador de base de datos devuelto a través del callback siempre es un nuevo manejador.
	// Si es necesario, se debe usar IsSameConnection() para probar la conexión contra otras.
	//
	// El manejador de resultados devuelto a través del callback es temporal y se destruye al final del callback.
	//
	// @param callback       Callback.
	// @param query          Cadena de consulta.
	// @param data           Valor de datos extra para pasar al callback.
	// @param prio           Cola de prioridad a usar.
	public native void Query(SQLQueryCallback callback, const char[] query,
	                         any data = 0,
	                         DBPriority prio = DBPrio_Normal);

	// Envía una transacción al hilo de base de datos. El manejador de transacción se
	// cierra automáticamente. Cuando la transacción completa, se invoca el callback opcional.
	//
	// @param txn            Un manejador de transacción.
	// @param onSuccess      Un callback opcional para recibir una transacción exitosa.
	// @param onError        Un callback opcional para recibir un mensaje de error.
	// @param data           Un valor opcional para pasar a los callbacks.
	// @param prio           Cola de prioridad a usar.
	public native void Execute(Transaction txn,
	                           SQLTxnSuccess onSuccess = INVALID_FUNCTION,
	                           SQLTxnFailure onError = INVALID_FUNCTION,
	                           any data = 0,
	                           DBPriority priority = DBPrio_Normal);
};

/**
 * Crea una conexión SQL desde una configuración con nombre.
 *
 * @param confname      Nombre de configuración.
 * @param persistent    Verdadero para reutilizar una conexión persistente anterior si
 *                      es posible, falso en caso contrario.
 * @param error         Buffer de error.
 * @param maxlength     Longitud máxima del buffer de error.
 * @return              Un manejador de conexión a base de datos o INVALID_HANDLE en caso de fallo.
 */
native Database SQL_Connect(const char[] confname, bool persistent, char[] error, int maxlength);

/**
 * Crea una conexión SQL predeterminada.
 *
 * @param error         Buffer de error.
 * @param maxlength     Longitud máxima del buffer de error.
 * @param persistent    Verdadero para reutilizar una conexión persistente anterior
 *                      si es posible, falso en caso contrario.
 * @return              Un manejador de conexión a base de datos o INVALID_HANDLE en caso de fallo.
 *                      En caso de fallo, el buffer de error contendrá un mensaje.
 */
stock Database SQL_DefConnect(char[] error, int maxlength, bool persistent=true)
{
	return SQL_Connect("default", persistent, error, maxlength);
}

/**
 * Conecta a una base de datos usando pares clave/valor que contienen la información de la base de datos.
 * Los pares clave/valor deben coincidir con los que estarían en databases.cfg.
 *
 * Por ejemplo, "driver" debe ser "default" o el nombre de un controlador (u omitirse para 
 * el predeterminado). Para SQLite, solo se necesita el parámetro "database".
 * Para controladores que requieren conexiones externas, pueden necesitarse más parámetros.
 *
 * En general, se desaconseja el uso de esta función. Las conexiones deben pasar por 
 * databases.cfg para mayor flexibilidad a favor de los usuarios.
 *
 * @param keyvalues     Pares clave/valor de un manejador KeyValues que describe la conexión.
 * @param error         Buffer de error.
 * @param maxlength     Longitud máxima del buffer de error.
 * @param persistent    Verdadero para reutilizar una conexión persistente anterior
 *                      si es posible, falso en caso contrario.
 * @return              Un manejador de conexión a base de datos o INVALID_HANDLE en caso de fallo.
 *                      En caso de fallo, el buffer de error contendrá un mensaje.
 * @error               Manejador KeyValues inválido.
 */
native Database SQL_ConnectCustom(Handle keyvalues,
								  char[] error,
								  int maxlength,
								  bool persistent);

/**
 * Obtiene un manejador de una base de datos SQLite, creándola si no existe.
 * 
 * Como precaución, siempre se debe crear un prefijo único en los nombres de tabla 
 * para evitar conflictos, y nunca se deben eliminar o modificar tablas que no sean propias.
 *
 * @param database      Nombre de la base de datos.  
 * @param error         Buffer de error.
 * @param maxlength     Longitud máxima del buffer de error.
 * @return              Un manejador de conexión a base de datos o INVALID_HANDLE en caso de fallo.
 *                      En caso de fallo, el buffer de error contendrá un mensaje.
 */
stock Database SQLite_UseDatabase(const char[] database, char[] error, int maxlength)
{
	KeyValues kv = new KeyValues("");
	kv.SetString("driver", "sqlite");
	kv.SetString("database", database);

	Database db = SQL_ConnectCustom(kv, error, maxlength, false);

	delete kv;

	return db;
}

/**
 * Función obsoleta. Usa SQL_ConnectCustom o SQLite_UseDatabase en su lugar.
 * @deprecated
 */
#pragma deprecated Use SQL_ConnectCustom instead.
native Handle SQL_ConnectEx(Handle driver, 
							const char[] host,
							const char[] user, 
							const char[] pass,
							const char[] database,
							char[] error,
							int maxlength,
							bool persistent=true,
							int port=0,
							int maxTimeout=0);
							
/**
 * Devuelve si existe una configuración con nombre en databases.cfg.
 *
 * @param name          Nombre de la configuración.
 * @return              Verdadero si existe, falso en caso contrario.
 */
native bool SQL_CheckConfig(const char[] name);

/**
 * Devuelve un manejador de controlador desde una cadena de nombre.
 *
 * Si no se encuentra el controlador, SourceMod intentará
 * cargar una extensión llamada dbi.<name>.ext.[dll|so].
 *
 * @param name          Cadena de identificación del controlador, o una cadena vacía
 *                      para devolver el controlador predeterminado.
 * @return              Manejador del controlador o INVALID_HANDLE en caso de fallo.
 */
native DBDriver SQL_GetDriver(const char[] name="");

/**
 * Lee el controlador de una base de datos abierta.
 *
 * @param database      Manejador de la base de datos.
 * @param ident         Buffer opcional para almacenar la cadena de identificación.
 * @param ident_length  Longitud máxima del buffer.
 * @return              Manejador del controlador.
 */
native DBDriver SQL_ReadDriver(Handle database, char[] ident="", int ident_length=0);

/**
 * Recupera la cadena de identificación de un controlador.
 *
 * Ejemplo: "mysql", "sqlite"
 *
 * @param driver        Manejador del controlador o INVALID_HANDLE para el controlador predeterminado.
 * @param ident         Buffer de la cadena de identificación.
 * @param maxlength     Longitud máxima del buffer.
 * @error               Manejador inválido distinto de INVALID_HANDLE.
 */
native void SQL_GetDriverIdent(Handle driver, char[] ident, int maxlength);

/**
 * Recupera la cadena de producto de un controlador.
 *
 * Ejemplo: "MySQL", "SQLite"
 *
 * @param driver        Manejador del controlador o INVALID_HANDLE para el controlador predeterminado.
 * @param product       Buffer de la cadena del producto.
 * @param maxlength     Longitud máxima del buffer.
 * @error               Manejador inválido distinto de INVALID_HANDLE.
 */
native void SQL_GetDriverProduct(Handle driver, char[] product, int maxlength);

/**
 * Establece el conjunto de caracteres de la conexión actual.
 * Similar a SET NAMES en mysql, pero permanece después de problemas de conexión.
 * 
 * Ejemplo: "utf8", "latin1"
 *
 * @param database      Manejador de la base de datos.
 * @param charset       Cadena del conjunto de caracteres al que cambiar.
 * @return              Verdadero si se cambió el conjunto de caracteres, falso en caso contrario.
 */
native bool SQL_SetCharset(Handle database, const char[] charset);

/**
 * Devuelve el número de filas afectadas por la última consulta.
 *
 * @param hndl          Un manejador de base de datos o de declaración.
 * @return              Número de filas afectadas por la última consulta.
 * @error               Manejador de base de datos o de declaración no válido.
 */
native int SQL_GetAffectedRows(Handle hndl);

/**
 * Devuelve el ID de inserción de la última consulta.
 *
 * @param hndl          Un manejador de base de datos, consulta o declaración.
 * @return              ID de inserción de la última consulta.
 * @error               Manejador de base de datos, consulta o declaración no válido.
 */
native int SQL_GetInsertId(Handle hndl);

/**
 * Devuelve el error reportado por la última consulta.
 *
 * @param hndl          Un manejador de base de datos, consulta o declaración.
 * @param error         Buffer de error.
 * @param maxlength     Longitud máxima del buffer.
 * @return              Verdadero si hubo un error, falso en caso contrario.
 * @error               Manejador de base de datos, consulta o declaración no válido.
 */
native bool SQL_GetError(Handle hndl, char[] error, int maxlength);

/**
 * Escapa una cadena de base de datos para su inserción literal. Esto no es necesario
 * para asociar cadenas en declaraciones preparadas.
 *
 * Generalmente, las cadenas de base de datos se insertan en consultas entre comillas simples (').
 * Si la entrada del usuario tiene una comilla simple, debe escaparse. Esta función asegura
 * que cualquier carácter inseguro esté adecuadamente escapado según el motor de base de datos
 * y el conjunto de caracteres de la base de datos.
 *
 * NOTA: SourceMod solo garantiza que las cadenas estén correctamente escapadas cuando la
 * consulta encierra la cadena entre comillas simples. Aunque los controladores tienden a
 * permitir comillas dobles ("), la cadena podría no estar escapada (por ejemplo, en SQLite).
 *
 * @param database      Un manejador de base de datos.
 * @param string        Cadena para escapar.
 * @param buffer        Buffer para almacenar la cadena escapada.
 * @param maxlength     Longitud máxima del buffer.
 * @param written       Opcionalmente devuelve el número de bytes escritos.
 * @return              Verdadero en éxito, falso si el buffer no es lo suficientemente grande.
 *                      El buffer debe ser al menos 2*strlen(string)+1.
 * @error               Manejador de base de datos o declaración no válido.
 */
native bool SQL_EscapeString(Handle database, 
							 const char[] string, 
							 char[] buffer, 
							 int maxlength, 
							 int &written=0);

/**
 * Formatea una cadena de acuerdo con las reglas de formato de SourceMod (consulta la documentación).
 * Todos los especificadores de formato se escapan (consulta SQL_EscapeString) a menos que se use el indicador '!'.
 *
 * @param database      Un manejador de base de datos.
 * @param buffer        Buffer de cadena de destino.
 * @param maxlength     Longitud máxima del buffer de salida.
 * @param format        Reglas de formato.
 * @param ...           Número variable de parámetros de formato.
 * @return              Número de celdas escritas.
 */
native int SQL_FormatQuery(Handle database, char[] buffer, int maxlength, const char[] format, any ...);

/**
 * Esta función está obsoleta. Usa SQL_EscapeString en su lugar.
 * @deprecated
 */
#pragma deprecated Use SQL_EscapeString instead.
stock bool SQL_QuoteString(Handle database,
						   const char[] string,
						   char[] buffer,
						   int maxlength,
						   int &written=0)
{
	return SQL_EscapeString(database, string, buffer, maxlength, written);
}

/**
 * Ejecuta una consulta y omite el conjunto de resultados.
 *
 * @param database      Un manejador de base de datos.
 * @param query         Cadena de consulta.
 * @param len           Parámetro opcional para especificar la longitud de la consulta, en
 *                      bytes. Esto se puede usar para enviar consultas binarias con terminadores prematuros.
 * @return              Verdadero si la consulta tuvo éxito, falso en caso contrario. Usa
 *                      SQL_GetError para encontrar el último error.
 * @error               Manejador de base de datos no válido.
 */
native bool SQL_FastQuery(Handle database, const char[] query, int len=-1);

/**
 * Ejecuta una consulta simple y devuelve un nuevo manejador de consulta para
 * recibir los resultados.
 *
 * @param database      Un manejador de base de datos.
 * @param query         Cadena de consulta.
 * @param len           Parámetro opcional para especificar la longitud de la consulta, en 
 *                      bytes. Esto se puede usar para enviar consultas binarias con terminadores prematuros.
 * @return              Un nuevo manejador de consulta en caso de éxito o INVALID_HANDLE 
 *                      en caso de fallo. El manejador debe liberarse con CloseHandle().
 * @error               Manejador de base de datos no válido.
 */
native DBResultSet SQL_Query(Handle database, const char[] query, int len=-1);

/**
 * Crea una nueva consulta preparada. Las declaraciones preparadas pueden
 * ejecutarse varias veces. También pueden tener parámetros de marcador de posición,
 * similares a variables, que se pueden enlazar de forma segura (por ejemplo, no es
 * necesario escapear las cadenas enlazadas).
 *
 * Los manejadores de declaraciones funcionarán en cualquier función que acepte un manejador de consulta.
 *
 * @param database      Un manejador de base de datos.
 * @param query         Cadena de consulta.
 * @param error         Buffer de error.
 * @param maxlength     Tamaño máximo del buffer de error.
 * @return              Un nuevo manejador de declaración en caso de éxito o INVALID_HANDLE
 *                      en caso de fallo. El manejador debe liberarse con CloseHandle().
 * @error               Manejador de base de datos no válido.
 */
native DBStatement SQL_PrepareQuery(Handle database, const char[] query, char[] error, int maxlength);

/**
 * Avanza al siguiente conjunto de resultados.
 *
 * En algunas implementaciones de SQL, pueden existir múltiples conjuntos de resultados en una consulta.
 * Esto es posible en MySQL con consultas simples cuando se ejecuta una consulta CALL.
 * Si este es el caso, todos los conjuntos de resultados deben procesarse antes de hacer otra consulta.
 *
 * @param query         Un manejador de consulta.
 * @return              Verdadero si había otro conjunto de resultados, falso en caso contrario.
 * @error               Manejador de consulta no válido.
 */
native bool SQL_FetchMoreResults(Handle query);

/**
 * Devuelve si existe o no un conjunto de resultados. Esto devolverá
 * verdadero incluso si se devolvieron 0 resultados, pero falso en consultas como
 * UPDATE, INSERT o DELETE.
 *
 * @param query         Un manejador de consulta (o declaración).
 * @return              Verdadero si hay un conjunto de resultados, falso en caso contrario.
 * @error               Manejador de consulta no válido.
 */
native bool SQL_HasResultSet(Handle query);

/**
 * Recupera el número de filas en el último conjunto de resultados.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @return              Número de filas en el conjunto de resultados actual.
 * @error               Manejador de consulta no válido.
 */
native int SQL_GetRowCount(Handle query);

/**
 * Recupera el número de campos en el último conjunto de resultados.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @return              Número de campos en el conjunto de resultados actual.
 * @error               Manejador de consulta no válido.
 */
native int SQL_GetFieldCount(Handle query);

/**
 * Recupera el nombre de un campo por su índice.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @param field         Número del campo (comenzando desde 0).
 * @param name          Buffer para el nombre.
 * @param maxlength     Longitud máxima del buffer del nombre.
 * @error               Manejador de consulta no válido, índice de campo no válido
 *                      o sin conjunto de resultados actual.
 */
native void SQL_FieldNumToName(Handle query, int field, char[] name, int maxlength);

/**
 * Recupera el índice de un campo por su nombre.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @param name          Nombre del campo (sensible a mayúsculas y minúsculas).
 * @param field         Variable para almacenar el índice del campo.
 * @return              Verdadero si se encontró, falso en caso contrario.
 * @error               Manejador de consulta no válido o sin conjunto de resultados actual.
 */
native bool SQL_FieldNameToNum(Handle query, const char[] name, int &field);

/**
 * Obtiene una fila del conjunto de resultados actual. Esto debe ser
 * llamado exitosamente antes de recuperar cualquier resultado.
 *
 * Si esta función falla, SQL_MoreRows() puede usarse para verificar si hubo un error
 * o si el conjunto de resultados ha terminado.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @return              Verdadero si se obtuvo una fila, falso en caso contrario.
 * @error               Manejador de consulta no válido.
 */
native bool SQL_FetchRow(Handle query);

/**
 * Indica si hay más filas.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @return              Verdadero si hay más filas, falso en caso contrario.
 * @error               Manejador de consulta no válido.
 */
native bool SQL_MoreRows(Handle query);

/**
 * Retrocede al primer resultado en un conjunto de resultados.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @return              Verdadero en éxito, falso en caso contrario.
 * @error               Manejador de consulta no válido o sin conjunto de resultados actual.
 */
native bool SQL_Rewind(Handle query);

/**
 * Recupera una cadena de un campo en la fila actual de un conjunto de resultados.  
 * Si el resultado es NULL, se devolverá una cadena vacía. Una verificación de NULL 
 * puede realizarse con el parámetro de resultado o con SQL_IsFieldNull().
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @param field         El índice del campo (comenzando desde 0).
 * @param buffer        Buffer para la cadena.
 * @param maxlength     Tamaño máximo del buffer para la cadena.
 * @param result        Variable opcional para almacenar el estado del valor devuelto.
 * @return              Número de bytes escritos.
 * @error               Manejador de consulta no válido o índice de campo no válido, 
 *                      conversión de tipo no válida desde la base de datos o sin conjunto de resultados actual.
 */
native int SQL_FetchString(Handle query, int field, char[] buffer, int maxlength, DBResult &result=DBVal_Error);

/**
 * Recupera un flotante de un campo en la fila actual de un conjunto de resultados.
 * Si el resultado es NULL, se devolverá un valor de 0.0. Una verificación de NULL 
 * puede realizarse con el parámetro de resultado o con SQL_IsFieldNull().
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @param field         El índice del campo (comenzando desde 0).
 * @param result        Variable opcional para almacenar el estado del valor devuelto.
 * @return              Un valor flotante.
 * @error               Manejador de consulta no válido o índice de campo no válido, 
 *                      conversión de tipo no válida desde la base de datos o sin conjunto de resultados actual.
 */
native float SQL_FetchFloat(Handle query, int field, DBResult &result=DBVal_Error);

/**
 * Recupera un entero de un campo en la fila actual de un conjunto de resultados.
 * Si el resultado es NULL, se devolverá un valor de 0. Una verificación de NULL 
 * puede realizarse con el parámetro de resultado o con SQL_IsFieldNull().
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @param field         El índice del campo (comenzando desde 0).
 * @param result        Variable opcional para almacenar el estado del valor devuelto.
 * @return              Un valor entero.
 * @error               Manejador de consulta no válido o índice de campo no válido, 
 *                      conversión de tipo no válida desde la base de datos o sin conjunto de resultados actual.
 */
native int SQL_FetchInt(Handle query, int field, DBResult &result=DBVal_Error);

/**
 * Indica si los datos de un campo en la fila actual de un conjunto de resultados son
 * NULL o no. NULL es un tipo de SQL que significa "sin datos".
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @param field         El índice del campo (comenzando desde 0).
 * @return              Verdadero si los datos son NULL, falso en caso contrario.
 * @error               Manejador de consulta o índice de campo no válido o sin conjunto de resultados actual.
 */
native bool SQL_IsFieldNull(Handle query, int field);

/**
 * Devuelve la longitud de los datos de un campo en la fila actual de un conjunto de resultados.
 * Esto solo necesita llamarse para cadenas para determinar cuántos bytes usar.
 * Ten en cuenta que el valor devuelto no incluye el terminador nulo.
 * 
 * @param query         Un manejador de consulta (o declaración).
 * @param field         El índice del campo (comenzando desde 0).
 * @return              Número de bytes del tamaño de los datos del campo.
 * @error               Manejador de consulta no válido, índice de campo no válido o sin conjunto de resultados actual.
 */
native int SQL_FetchSize(Handle query, int field);

/**
 * Asocia un parámetro en una declaración preparada a un valor entero.
 * 
 * @param statement     Un manejador de declaración preparada.
 * @param param         Índice del parámetro (comenzando desde 0).
 * @param number        Número a asociar.
 * @param signed        Verdadero para asociar el número como firmado, falso para 
 *                      asociarlo como sin signo.
 * @error               Manejador de declaración o índice de parámetro no válido o error SQL.
 */
native void SQL_BindParamInt(Handle statement, int param, int number, bool signed=true);

/**
 * Asocia un parámetro en una declaración preparada a un valor flotante.
 * 
 * @param statement     Un manejador de declaración preparada.
 * @param param         Índice del parámetro (comenzando desde 0).
 * @param value         Número flotante a asociar.
 * @error               Manejador de declaración o índice de parámetro no válido o error SQL.
 */
native void SQL_BindParamFloat(Handle statement, int param, float value);

/**
 * Asocia un parámetro en una declaración preparada a un valor de cadena.
 * 
 * @param statement     Un manejador de declaración preparada.
 * @param param         Índice del parámetro (comenzando desde 0).
 * @param value         Cadena a asociar.
 * @param copy          Si SourceMod debe copiar el valor localmente si es necesario.
 *                      Si el contenido de la cadena no cambiará antes de llamar a
 *                      SQL_Execute(), esto puede establecerse en falso para optimización.
 * @error               Manejador de declaración o índice de parámetro no válido o error SQL.
 */
native void SQL_BindParamString(Handle statement, int param, const char[] value, bool copy);

/**
 * Ejecuta una declaración preparada. Todos los parámetros deben estar asociados de antemano.
 *
 * @param statement     Un manejador de declaración preparada.
 * @return              Verdadero en éxito, falso en caso de fallo.
 * @error               Manejador de declaración no válido.
 */
native bool SQL_Execute(Handle statement);

/**
 * Bloquea una base de datos para que las operaciones en subproceso no interrumpan.
 * 
 * Si estás utilizando un manejador de base de datos para operaciones en subproceso y no en subproceso,
 * DEBES llamar a esta función antes de realizar operaciones sin subproceso.
 * De lo contrario, podrías corromper la memoria o conexión de la base de datos.
 * 
 * Dejar un bloqueo en la base de datos y luego ejecutar una consulta en subproceso
 * resulta en un bloqueo total. Asegúrate de llamar a SQL_UnlockDatabase().
 *
 * @param database      Un manejador de base de datos.
 * @error               Manejador de base de datos no válido.
 */
native void SQL_LockDatabase(Handle database);

/**
 * Desbloquea una base de datos para que las operaciones en subproceso puedan continuar.
 *
 * @param database      Un manejador de base de datos.
 * @error               Manejador de base de datos no válido.
 */
native void SQL_UnlockDatabase(Handle database);

/**
 * Callback general para operaciones SQL en subproceso.
 * 
 * @param owner         Objeto padre del manejador (o INVALID_HANDLE si no hay ninguno).
 * @param hndl          Manejador del objeto secundario (o INVALID_HANDLE si no hay ninguno).
 * @param error         Cadena de error si hubo un problema. El error podría estar 
 *                      vacío incluso si hay una condición de error, por lo que es importante 
 *                      verificar el valor real del manejador.
 * @param data          Datos pasados en la invocación original en subproceso.
 */
typedef SQLTCallback = function void (Handle owner, Handle hndl, const char[] error, any data);

/**
 * Indica si dos manejadores de base de datos apuntan a la misma conexión de base de datos.
 *
 * @param hndl1         Primer manejador de base de datos.
 * @param hndl2         Segundo manejador de base de datos.
 * @return              Verdadero si los manejadores apuntan a la misma conexión, falso en caso contrario.
 * @error               Manejador no válido.
 */
native bool SQL_IsSameConnection(Handle hndl1, Handle hndl2);

/**
 * Conecta a una base de datos a través de un subproceso. Esto se puede usar en lugar de
 * SQL_Connect() si se desea funcionalidad sin bloqueo.
 *
 * No es necesario usar esto para usar consultas en subproceso. Sin embargo, si no se usa 
 * (o si se mezclan consultas en subproceso y sin subproceso), consulta SQL_LockDatabase().
 *
 * @param callback      Callback; el nuevo manejador estará en hndl, el propietario es el controlador.
 *                      Si no se encontró el controlador, el propietario es INVALID_HANDLE.
 * @param name          Nombre de la base de datos.
 * @param data          Valor de datos extra para pasar al callback.
 */
native void SQL_TConnect(SQLTCallback callback, const char[] name="default", any data=0);

/**
 * Ejecuta una consulta simple a través de un subproceso. El manejador de consulta se pasa a través del callback.
 *
 * El manejador de base de datos devuelto a través del callback siempre es un nuevo manejador.
 * Si es necesario, SQL_IsSameConnection() debe usarse para probar la conexión contra otras.
 *
 * El manejador de consulta devuelto a través del callback es temporal y se destruye al final del callback.
 * Si necesitas mantenerlo, usa CloneHandle().
 *
 * @param database      Un manejador de base de datos.
 * @param callback      Callback; la base de datos está en "owner" y el manejador de consulta se pasa en "hndl".
 * @param query         Cadena de consulta.
 * @param data          Valor de datos extra para pasar al callback.
 * @param prio          Cola de prioridad a usar.
 * @error               Manejador de base de datos no válido.
 */
native void SQL_TQuery(Handle database, SQLTCallback callback, const char[] query, any data=0, DBPriority prio=DBPrio_Normal);

/**
 * Crea un nuevo objeto de transacción. Un objeto de transacción es una lista de consultas
 * que pueden enviarse al hilo de la base de datos y ejecutarse como una sola transacción.
 *
 * @return              Un manejador de transacción.
 */
native Transaction SQL_CreateTransaction();

/**
 * Añade una consulta a un objeto de transacción.
 *
 * @param txn           Un manejador de transacción.
 * @param query         Cadena de consulta.
 * @param data          Valor de datos extra para pasar al callback final.
 * @return              Índice de la consulta en la lista de consultas de la transacción.
 * @error               Manejador de transacción no válido.
 */
native int SQL_AddQuery(Transaction txn, const char[] query, any data=0);

/**
 * Envía una transacción al hilo de la base de datos. El manejador de transacción se
 * cierra automáticamente. Cuando la transacción se completa, se invoca el callback opcional.
 *
 * @param db            Un manejador de base de datos.
 * @param txn           Un manejador de transacción.
 * @param onSuccess     Un callback opcional para recibir una transacción exitosa.
 * @param onError       Un callback opcional para recibir un mensaje de error.
 * @param data          Un valor opcional para pasar a los callbacks.
 * @param prio          Cola de prioridad a usar.
 * @error               Manejador no válido.
 */
native void SQL_ExecuteTransaction(
		Handle db,
		Transaction txn,
		SQLTxnSuccess onSuccess = INVALID_FUNCTION,
		SQLTxnFailure onError = INVALID_FUNCTION,
		any data=0,
		DBPriority priority=DBPrio_Normal);
