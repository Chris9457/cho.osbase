# Service API definition

## Lexical definition

### **Tags**

| Name tag        | Description                                                   |
| --------------- | ------------------------------------------------------------- |
| service         | Main block for the service description                        |
| name            | Name of the element                                           |
| description     | Description of the element                                    |
| realm           | Realm of the service (cf WAMP)                                |
| namespace       | Namespace of the service                                      |
| domain_objects  | Main block for the domain objects description                 |
| struct          | "Container" domain object tag (set of fields)                 |
| enum            | "Enumerate" domain object tag (set of values of a same group) |
| alias           | "Alias" domain object tag (synonym)                           |
| constant        | String constant                                               |
| fields          | Fields of the "struct" domain object                          |
| values          | Values of the "enum" domain object                            |
| value           | Value of an element                                           |
| type            | Type of the element                                           |
| process         | Main block for the process description                        |
| uri             | Unique Resource Identifier (URI) of the process element       |
| arguments       | Arguments of the process element                              |
| const           | Constant modifier of the process element                      |
| events          | Main block for the events description                         |
| topic           | Topic of the event                                            |
| imports         | Imports section                                               |
| module          | Module section                                                |

### **Types**
(`async_access`)`type_name`[`nb_occurrence`]?

(`async_access`) : optional - indicate if the access of the value is asynchroneous. Possible values of `async_access`:
   - `async`: one frame asynchroneous value 
     - JSON type equivalence: string (uri content)
     - c++ class equivalence : AsyncData<`type_name`>
   - `async_paged`: paged frame asynchroneous value 
     - JSON type equivalence: string (uri content)
     - c++ class equivalence : AsyncPagedData<`type_name`>

`type_name` : mandatory - name of the type. This name can be either:
   - a predefined name (see table below)
   - a domain object (see section `domain objects`)

[`nb_occurrence`] : optional - Indicate that the type is collection:
   - fixed if `nb_occurrence` is filled (ex: integer[4] ==> collection of strictly 4 integers) 
     - JSON type equivalence: array
     - c++ class equivalence: std::array<`type_name`, `nb_occurrence`>
   - dynamic if `nb_occurrence` is not filled (ex: integer[] ==> collection of integers, length not fixed) 
     - JSON type equivalence: array
     - c++ class equivalence: std::vector<`type_name`>

? : optional - Inidicate if the type is optional
   - JSON type equivalence


| Type name                 | Description                                                   |  JSON Type        | CPP type                      |
| ------------------------- | ------------------------------------------------------------- | ----------------- | ----------------------------- |
| char				        | Signed numeric integer type (1 byte)                          | number            | char                          |
| unsigned char	            | Unsigned numeric integer type (1 byte)                        | number            | unsigned char                 |
| short integer             | Signed numeric integer type (2 bytes)                         | number            | short                         |
| unsigned short integer    | Unsigned numeric integer type (2 bytes)                       | number            | unsigned short                |
| integer                   | Signed numeric integer type (4 bytes)                         | number            | int                           |
| unsigned integer          | Unsigned numeric integer type(4 bytes)                        | number            | unsigned int                  |
| long integer              | Signed numeric integer type (8 bytes)                         | number            | long long                     |
| unsigned long integer     | Unsigned numeric integer type (8 bytes)                       | number            | unsigned long long            |
| short floating            | Numeric floating type(4 bytes)                                | number            | float                         |
| floating                  | Numeric floating type (8 bytes)                               | number            | double                        |
| string                    | String type (set of character) - ex: "toto"                   | string            | std::string                   |
| boolean                   | Boolean type (true or false)                                  | boolean           | bool                          |
| uri                       | Uri type (special string that represent a value URI)          | string            | osbase::data::Uri        | 


## Grammatical definition

The API has the following group at the 1st level

| Fields           | [M]andatory / [O]ptional | Description                                                   |
| ---------------- | ------------------------ | ------------------------------------------------------------- |
| service / module |            M             | Global information of the service / module                    |
| imports          |            O             | List of the import files                                      |
| domain_objects   |            O             | List of the domain objects                                    |
| process          |            O             | List of the process (only for service)                        |
| events           |            O             | List of the events (only for service)                         |

### **service**

Contains the global information of the service. When this section is present, the section `module` is not allowed 

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| name            |            M             | Name of the service                                           |
| description     |            O             | Description of the service                                    |
| realm           |            M             | Relam of the service (cf WAMP)                                |
| namespace       |            O             | Namespace of the service                                      |
| constants       |            O             | Constants of the service (string values)                      |

#### Example
    service:
        name: "ITest"
        description: "api of the test service"
        realm: "osbase"
        namespace: "testservice.api"
        constants:
            - name: "testServiceChannelSagitalViewChannel"
              value: "SagitalViewChannel"
              description: "channel name for Sagittal view"

### **module**

Contains the global information of the module. When this section is present, the section `service` is not allowed 

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| name            |            M             | Name of the module                                            |
| description     |            O             | Description of the module                                     |
| namespace       |            O             | Namespace of the module                                       |

#### Example
    module:
      name: "testImport"
      description: "one import"
      namespace: "testservice.api.sub"

#### **service/constants**
| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| name            |            M             | Name of the constant                                          |
| description     |            O             | Description of the constant                                   |
| value           |            M             | Value of the constant (string value)					         |

##### Example
    constants:
        - name: "testServiceChannelSagitalViewChannel"
          value: "SagitalViewChannel"
          description: "channel name for Sagittal view"


### **imports**

Contains the list of the import files. Note each import file contain a section module (and not service)

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| name            |            M             | Name / path of the module to import                           |

#### Example
    imports:
      - name: "testimport.yml"
      - name: "testimport2.yml"


### **domain objects**

Contains the list of the domain objects. The categories are:

| Category        | Description                                                   |
| --------------- | ------------------------------------------------------------- |
| struct          | Container of information                                      |
| enum            | Group of values                                               |
| alias           | Synonym of another type                                       |

#### Example
    domain_objects:
      - struct: "Position"
        ...
      - alias: "Positions"
        ...
      - enum: "EPosition"
        ...

#### **domain_objects/struct**

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| struct          |            M             | Name of the struct                                            |
| description     |            O             | Description of the struct                                     |
| fields          |            O             | Fields      of the struct                                     |

#### **domain_objects/struct/fields**

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| name            |            M             | Name of the field                                             |
| type            |            M             | Type of the field                                             |
| value           |            O             | Initialization value of the field                             |
| description     |            O             | Description of the field                                      |

##### Example
    - struct: "Position"
        description: "3D position"
        fields:
          - name: "x"
            type: "floating"
            description: "x-axe value"
            value: "0.0"
          - name: "y"
            type: "floating"
            description: "y-axe value of the pixel"
         - name: "z"
            type: "floating"
            description: "z-axe value of the pixel"

#### **domain_objects/alias**

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| alias           |            M             | Name of the alias                                             |
| type            |            M             | Type of alias                                                 |
| description     |            O             | Description of the alias                                      |

##### Example
    - alias: "Positions"
      description: "collection of postions"
      type: Position[]

#### **domain_objects/enum**

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| enum            |            M             | Name of the enum                                              |
| description     |            O             | Description of the struct                                     |
| values          |            O             | Values of the enum                                            |

#### **domain_objects/enum/values**

| Fields          | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| name            |            M             | Name of the value                                             |
| description     |            O             | Description of the value                                      |
| value           |            O             | (integer) value of the value                                  |

##### Example
    - enum: "EPosition"
        description: "Absolute or relative"
        values:
          - name: "Relative"
            value: "0"
            description: "Relative position"
          - name: "Absolute"
            value: 1
            description: "Absolute position"

### **Process**

Contains the list of the process (available only for service). The following table describes an element of this list:

| Fields          | [M]andatory / [O]ptional | Description                                                       |
| --------------- | ------------------------ | ----------------------------------------------------------------- |
| name            |            M             | Name of the process                                               |
| description     |            O             | Description of the process                                        |
| type            |            O             | Return type of the process - If not (or none), no return          |
| const           |            O             | Const modifier type - if not set as false                         |
| uri             |            M             | Uri of the process                                                |
| arguments       |            O             | List of the arguments of the process                              |

#### **process/arguments**

| Arguments       | [M]andatory / [O]ptional | Description                                                   |
| --------------- | ------------------------ | ------------------------------------------------------------- |
| name            |            M             | Name of the argument                                          |
| type            |            M             | Type of the argument                                          |
| description     |            O             | Description of the argument                                   |

#### Example
    process:
      - name: "getTransfo"
        type: "Mat4"
        arguments:
          - name: "origin"
            type: "Vec4"
          - name: "dest"
            type: "Vec4"
      ...

### **Events**

Contains the list of the events (available only for service). The following table describes an element of this list:

| Fields          | [M]andatory / [O]ptional | Description                                                       |
| --------------- | ------------------------ | ----------------------------------------------------------------- |
| name            |            M             | Name of the event                                                 |
| description     |            O             | Description of the event                                          |
| type            |            O             | Return type of the process - If not (or none), no return          |
| topic           |            M             | Topic of the event - if not set to "<service.name>Service.<name>" |

#### Example
    events:
      - name: "PositionUpdatedMsg"
        type: "Position"
        topic: "ITestService.PositionUpdatedMsg"
        description: "emit when the position is updated"
      ...
