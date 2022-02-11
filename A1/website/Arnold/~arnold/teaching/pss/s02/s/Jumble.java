import java.util.*;
import java.io.*;

class Jumble {
	// Key concepts: Once you understand these, the program is 
	// is simple, clear and obvious
	//
	// The normal form of String X is sort(lowerCase(X))
	//
	// The equivalence class of String X is the set of strings
	// with the same normal form. Each equivlaence class is
	// identified by the classes normal form.
	//
	// Theorem: X is a jumble of Y if and only if 
	// X and Y appear in the same equivalence class.
	//
	// The jumble problem is solved by the largest equivalence class
	// of the words in a dictionary.
	// 
	//
	// Return the normal form of w (see above)
	// that is lowerCase(X) then sort the resulting characters
	// example: normalize("bccDsA") is "abccds"
	public static String normalize(String w){
		w=w.toLowerCase();
		String nw="";
		int i,j;
		for(i=0;i<w.length();i++){
			char c=w.charAt(i);
			for(j=0;j<nw.length();j++){
				char nc=nw.charAt(j);
				if(c<nc)break;
			}
			nw=nw.substring(0,j)+c+nw.substring(j);
		}
		return(nw);
	}

	// Print all equivalence classes for file words
	// along with the size of each equivalence class
	public static void main(String [] args){
		try{
			Jumble j=new Jumble("words");
			System.out.println(j);
		} catch (Exception e){
			System.out.println(e);
		}
	}

		/* 
		 * The collection of equivalence classes is held in
		 * a TreeMap. This is like a Vector which can be indexed by objects
		 * instead of integers. We use the normal form for a class
		 * for the classes index.
		 */
	TreeMap tm;

	// Read all strings in dictionary fileName
	// and add each to its equivalence class
	Jumble(String fileName) throws Exception {
		tm=new TreeMap();

		FileReader fr=new FileReader(fileName);
                BufferedReader br=new BufferedReader(fr);

                String line; 
                while(true){
                        line=br.readLine();
                        if(line==null)break; 
			add(line);
                }
	}
	
	// add w to its equivalence class
	public void add(String w){
		/* Algorithm:
		 * normalize w
		 * find the equivalence class for w (using normalize(w))
		 * if one already exists, then add w to it
		 * otherwise create a new equivalence class
		 * 	with w as its only member.
		 */
		String n=normalize(w.toLowerCase());
                Vector v=(Vector)tm.get(n);
		if(v==null){
			v=new Vector();
			tm.put(n,v);
		}
		v.add(w);
	}

	// Return a String representation of all equivalence
	// classes. Each is prepended with its size
	public String toString(){ 
		StringBuffer rv=new StringBuffer();
		Set ks=tm.keySet();
		for(Iterator i=ks.iterator();i.hasNext();){
			String k=(String)i.next();
			Vector v=(Vector)tm.get(k);
			rv.append(v.size()+" "+k+"="+tm.get(k)+"\n");
		}
		return(rv.toString());
	}
}
