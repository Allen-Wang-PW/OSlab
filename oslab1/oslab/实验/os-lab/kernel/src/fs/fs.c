   zinvalid grouping)r   �
ValueError)r(   Zlast_interval�intervalr   r   r   �_grouping_intervals{   s    rE   Fc       
   
   C   sH  t  �  } | | r d p d } | | r. d p1 d } | sE |  d f S|  d
 d k r� |  j �  } |  t | � d  � } | }  n d } d } g  } xd t | � D]V }	 |  s� |  d d	 k r� |  } d }  Pn  | j |  |	 d  � � |  d  |	 � }  q� W|  r| j |  � n  | j �  | | j | � | t | � t | � d f S)Nr8   r7   r.   r(   r   r&   � r)   �
0123456789�����rH   )r	   �rstrip�lenrE   �append�reverse�join)
r   �monetary�convr7   r(   ZstrippedZright_spacesZleft_spaces�groupsrD   r   r   r   �_group�   s0    	
	
rQ   c             C   s�   d } x. | r6 |  | d k r6 | d 7} | d 8} q	 Wt  |  � d } x. | rw |  | d k rw | d 8} | d 8} qJ W|  | | d � S)Nr   rF   r&   )rJ   )r   ZamountZlposZrposr   r   r   �_strip_padding�   s    

rR   zG%(?:\((?P<key>.*?)\))?(?P<modifiers>[-#0-9 +*.hlL]*?)[eEfFgGdiouxXcrs%]c             G   sc   t  j |  � } | s4 t | j �  � t |  � k rM t d t |  � � � n  t |  | | | | � S)z�Returns the locale-aware substitution of a %? specifier
    (percent).

    additional is for format strings which contain one or more
    '*' modifiers.zHformat() must be given exactly one %%char format specifier, %s not valid)�_percent_re�matchrJ   �grouprC   �repr�_format)�percentr>   r(   rN   �
additionalrT   r   r   r   r   �   s
    %c       	      G   s  | r |  | f | } n
 |  | } |  d	 d k r� d } | j  d � } | rr t | d d | �\ | d <} n  t �  | r� d p� d } | j | � } | rt | | � } qnR |  d
 d k rd } | r� t | d | �\ } } n  | rt | | � } qn  | S)Nr&   ZeEfFgGr   r0   rN   r:   r1   ZdiurH   rH   )�splitrQ   r	   rM   rR   )	rX   r>   r(   rN   rY   Z	formattedZseps�partsr1   r   r   r   rW   �   s(    
#rW   c       	      C   so  t  t j |  � � } t j d |  � } t | t j � r� g  } x| D]K } | j �  d d k rr | j d � qF | j t	 | j �  | | � � qF Wn� t | t
 � s� | f } n  g  } d } x� | D]� } | j �  d	 d k r� | j d � q� | j d � j d � } | j t | j �  | | | d | | d | d | � � � | d | 7} q� Wt
 | � } | | S)
z�Formats a string in the same way that the % formatting would use,
    but takes the current locale into account.
    Grouping is applied if the third parameter is true.z%sr&   �%r   Z	modifiersr    FrH   rH   )�listrS   �finditer�sub�
isinstance�collections�MappingrU   rK   r   �tuple�countrW   )	�f�valr(   ZpercentsZnew_fZnew_valZperc�iZ	starcountr   r   r   r   �   s0    & Tc             C   s�  t  �  } | | r d p d } | d k r: t d � � n  t d | t |  � | d d �} d | d	 } | r� | | r d
 p� d } | |  d k  r� d p� d } | |  d k  r� d p� d }	 | r� | |	 r� d p� d | } q� | |	 r� d p� d | } n  | |  d k  rd pd }
 | |  d k  r/d p2d } |
 d k rSd | d } n~ |
 d k rl| | } ne |
 d k r�| | } nL |
 d k r�| j d | � } n+ |
 d k r�| j d	 | � } n
 | | } | j d d � j d	 d � S)zIFormats val according to the currency settings
    in the current locale.r;   r9   r!   z9Currency formatting is not possible using the 'C' locale.z%%.%ifrN   T�<�>r5   r*   r   r-   r,   r/   r4   rF   r)   r+   r6   r2   r3   �(�)r&   r'   r#   r%   )r	   rC   r   �abs�replace)rf   Zsymbolr(   ZinternationalrO   �digitsr   ZsmbZprecedesZ	separatedZsig