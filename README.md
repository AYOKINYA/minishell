## Parsing

### @minishell.c

go_minishell_go 

사실상 get_next_line loop.

get_next_line에서 받은 input을 exec합니다 ⇒ exec_input

### @exec_input.c

input을 우선 semicolon별로 split 합니다 by semicolon_split

semicolon_split된 inputs를 while loop으로 처리합니다.

loop 안에서 $?를 처리해주고 by dollar_question,

split_pipe_and_process_cmd에서 tokenize와 pipe_split을 합니다.

사실상 tokenize가 전처리 main part!

tokenize 후 pipe_split한 cmds를 process_cmd에서 처리합다!

process_cmd는 전처리 이후의 파트입니다.

### @tokenize.c

input을 처리하기 쉽게 토큰으로 쪼갭니다.

쪼갠 토큰을 쉽게 붙이기 위해 linked list를 사용했습니다. (t_list *tokens)

cmds_into_tokens에서는 큰 따옴표와 작은 따옴표 처리를 합니다.

cmds_into_tokens 설명은 @cmds_into_tokens.c를 참고해주세요.

tokens_into_args에서는 linked list인 tokens를 다루기 쉽게 2d array로 전환하는 동시에,

cmds_into_tokens에서 처리하지 않은 환경변수를 value로 치환합니다.

ex) 환경변수 $a=k일 때,

echo $a로 입력됐다면 echo k로 변환하여 2d array args를 만듭니다.

환경변수 치환은 convert_to_env_var에서 이뤄집니다.

### @convert_to_env_var.c

ex) $a=kkk일 때 hi$a가 input으로 들어왔을 때.

converted_len에서 환경변수로 치환했을 때의 길이를 잽니다.

hi$a → hikkk이므로 길이는 5입니다. 

coverted_len에서도, add_var에서도 문자열을 loop으로 돌려 $를 만나면  환경변수 리스트에서 a를 검색해 kkk를 찾아내어 $a가 아닌 kkk를 다룹니다.

따라서 convert_to_env_var의 return 값은 hikkk입니다.

### @cmds_into_tokens.c

quote_mark는 따옴표가 열리고 닫히는 것을 체크합니다.

read_token에서 따옴표 처리가 필요 없다면 read_unquoted_token이,

따옴표 처리를 해야 한다면 read_quoted_token이 실행됩니다.

두 함수의 골자는 비슷합니다. @read_quoted_token.c에서 read_quoted_token을 설명 드리겠습니다.

참고로 위 함수들의 return값은 작은 따옴표와 큰 따옴표로만 구분을 하게 되기 때문에 온전한 token이 아닙니다.  

ex) input이 "hi"j인 경우, "hij가 완전한 토큰인데, read_token은 quoted와 unquoted로 구분하여 "hi"j를 읽기 때문에 hi가 먼저 return되고, 그 다음 while이 실행될 때 j가 read_token에 의해 반환됩니다.

그래서 join_and_patch에서 온전한 token이 될 때까지 ft_strjoin으로 token을 이어 붙인 후 token이 완성되면 token_patch로 t_list *tokens에 token을 붙여줍니다. 예제에서는 hi와 j가 ft_strjoin된 후에서야 token이 patch됩니다.

### @read_quoted_token.c

★ 여기서는 환경변수 처리를 하지 않습니다! 특수문자와 따옴표 처리만!

ex) 환경변수가 $hi=kkk, input이 """$hi"""일 경우,

 $hi로 처리되어야 하므로 처음에 loop를 돌려 """를 읽고 $에 도달합니다.

(escape_excetion이 1일 때는 작은 따옴표이고 그 다음에 역슬래시가 올 때인데 echo '\\\'를 수정하다 보니 필요가 없어진 것 같습니다.) 

escape_excetion이 2일 때는 큰 따옴표고 그 다음에 바로 $나 "가 올 때입니다.

그리고 나서 quoted_token_len에서 $hi의 길이를 잽니다. 길이를 재기 위해 while문을 돌립니다.

큰 따옴표일 때는 작은 따옴표와 다르게 $ 기호가 정상적으로 작동하기 때문에 $이 단순 출력을 위한 $가 아니라,  제 기능을 하는 $라는 걸 표시하기 위해 $에 -1을 곱했습니다. 제 기능을하는 redirection 기호들 >, <, | 등에도 -1을 곱해 단순 출력과 차별화했습니다.

그리고 escape letter 역슬래시와 조합해 예외 케이스가 나오는 문자 (역슬래시, 달러 기호) 등을 고려해 문자열을 처리합니다 (by exception_substr)

= = = = = = = 

지난 만남으로 개선된 전처리

1.  echo "'$hi'"와 echo '''hi''' echo '"$hi"' 등 구현 제대로 되게함 

2.  echo hi"j"와 echo "hi"j, echo "hi" ' j ' 등 구현 제대로 되게함

3. echo '\\\' 시리즈 수정

4. semicolon error 처리

cf ) 전처리는 아니지만 sh_export 강화

전처리에서 해야 할 것

1.  echo "\$hi" ⇒ echo $hi로 출력된다. (escape letter 때문)
    이런 예외 경우가 더 있다면 잡아내야 한다.

2. 추후 기타 개선사항
